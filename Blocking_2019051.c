/*
Name: Kesar Shrivastava
Roll number: 2019051
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//It checks whether the size is available
void *check_malloc(int size)
	{
	  void *p = malloc (size);
	  if (p == NULL){ 
	  	perror("Error\n");
	  	exit(-1);
	  }
	  return p;
	}

typedef pthread_mutex_t Mutex ;


//Here I do nothing but only use pthread_mutex_t to be mutex.
//The functions described below is just redefining the functions with error values so that if no space can be allocated then error is shown.
Mutex * make_mutex ()
{
	Mutex * mutex = check_malloc ( sizeof ( Mutex ));
	int n = pthread_mutex_init ( mutex , NULL );
	if ( n != 0) {
		perror("Could not initialise mutex\n");
		exit(-1);
		}
	return mutex ;
}

void mutex_lock ( Mutex * mutex )
{
	int n = pthread_mutex_lock ( mutex );
	if ( n != 0) {
		perror("Could not lock mutex\n");
		exit(-1);}
}

void mutex_unlock ( Mutex * mutex )
{
	int n = pthread_mutex_unlock ( mutex );
	if ( n != 0) {
		perror("Could not unlock mutex\n");
		exit(-1);
	}
}

typedef pthread_cond_t Cond ;

//Here I have used conditional conditional pthread
//It is used so that in the semaphore implementation the variable threads wait on if they wait on semaphore.
//All the functions are defined so that no segmentation fault occurs.
Cond * make_cond ()
{
	Cond * cond = check_malloc ( sizeof ( Cond ));
	int n = pthread_cond_init ( cond , NULL );
	if (n != 0) {
		perror("Creation of conditional mutex failed\n");
		exit(-1);
	}
	return cond ;
}

void cond_wait ( Cond * cond , Mutex * mutex )
{
	int n = pthread_cond_wait ( cond , mutex );
	if (n != 0) {
		perror("Error\n");
		exit(-1);
	}
	
}

void cond_signal ( Cond * cond )
{
	int n = pthread_cond_signal ( cond );
	if (n != 0) {
		perror("Error\n");
		exit(-1);
	}
}

//This is the final implementation of semaphore.
//It has value, and the pthread library mutex, and the conditional mutex
//as described above so that the the variable threads wait on if they wait on semaphore.
//The reason for using wakeup to avoid a condition, where threads wait on a semaphore when thread executes signal, then one of the waiting threads should wake up.
typedef struct {
	int value , wakeups ;
	Mutex * my_semaphore ;
	Cond * cond ;
} Semaphore ;

//This is the sem_init function as described in the semaphore header file.
void make_semaphore ( Semaphore *semaphore, int value )
{
	semaphore -> value = value ;
	semaphore -> wakeups = 0;
	semaphore -> my_semaphore = make_mutex ();
	semaphore -> cond = make_cond ();
	
}

//sem_wait function
void sem_wait ( Semaphore * semaphore )
{
//lock the mutex
	mutex_lock ( semaphore -> my_semaphore );
	//decrement the value
	semaphore -> value --;

	if ( semaphore -> value < 0) {
		do {
		//do while loop so that the conditional mutex can be used
		//if non zero, value, then unlock and increment
			cond_wait ( semaphore -> cond , semaphore -> my_semaphore );
		} while ( semaphore -> wakeups < 1);
		semaphore -> wakeups --;
	}
	mutex_unlock ( semaphore -> my_semaphore );
}
void sem_post ( Semaphore * semaphore )
{
//there is no thread to give the signal, 
	mutex_lock ( semaphore -> my_semaphore );
	semaphore -> value ++;
	// no if condition
	if ( semaphore -> value <= 0) {
		semaphore -> wakeups ++;
		cond_signal ( semaphore -> cond );
	}
	mutex_unlock ( semaphore -> my_semaphore );
}

//the function so that the value of the semaphore can be checked.
void signal (Semaphore * semaphore){
	printf("The value is: %d\n", semaphore->value);
}

//global variable so that the value of k can be put by the user.
int k;

//two bowls named room as this is what I could understand
Semaphore room;
Semaphore room1;
Semaphore chopstick[5];

//Philosopher problem
void * philosopher(void *);
void eat(int);
int main()
{
	
	printf("Enter the value of k\n");
	scanf("%d", &k);
	int i,a[k];
	pthread_t tid[k];
	//printf("hello\n");
	
	make_semaphore(&room, k);
	make_semaphore(&room1, k);
	
	for(i=0;i<k;i++)
		make_semaphore(&chopstick[i], 1);
		
	//printf("hello\n");
		
	for(i=0;i<k;i++){
	
		a[i]=i;
		pthread_create(&tid[i],NULL,philosopher,(void *)&a[i]);
	}
	for(i=0;i<k;i++)
		pthread_join(tid[i],NULL);
	
}

//the dining philosopher problem.
void * philosopher(void * num)
{
while(1){
	int phil=*(int *)num;

	sem_wait(&room);
	sem_wait(&room1);
	//printf("P%d is thinking\n",phil);
	//signal(&room);
	//the deadlock is removed by allocating right fork first to P if P is even and vice-versa
	if(phil%2==0){
		sem_wait(&chopstick[(phil+1)%k]);
		//printf("P%d receives F%d\n",phil,(phil+1)%5);
		sem_wait(&chopstick[phil]);
		//printf("P%d receives F%d\n",phil,phil);
	}
	else{
		sem_wait(&chopstick[phil]);
		//printf("P%d receives F%d\n",phil,phil);
		sem_wait(&chopstick[(phil+1)%k]);
		
		//printf("P%d receives F%d\n",phil,(phil+1)%5);
	}
	eat(phil);
	sleep(2);
	//printf("P%d has finished eating\n",phil);

	sem_post(&chopstick[phil]);
	sem_post(&chopstick[(phil+1)%k]);
	sem_post(&room);
	sem_post(&room1);
}}

//function advocating eating activity.
void eat(int phil)
{
	printf("Philosopher %d eats using forks %d %d\n",phil, phil, (phil+1)%k);
}

