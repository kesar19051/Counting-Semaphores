/*
Name: Kesar Shriavstava
Roll no. 2019051
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//Instead of handling errors everywhere I have used a function.
void perror_exit (char *s)
	{
	  perror (s);  exit (-1);
	}
	
void *check_malloc(int size)
	{
	  void *p = malloc (size);
	  if (p == NULL) perror_exit ("Error");
	  return p;
	}

//All things are same as the blocking variant for the mutex structure.
typedef pthread_mutex_t Mutex ;

Mutex * make_mutex ()
{
	Mutex * mutex = check_malloc ( sizeof ( Mutex ));
	int n = pthread_mutex_init ( mutex , NULL );
	if ( n != 0) {perror("Could not create the mutex");
		exit(-1);
	}
	return mutex ;
}

//here I have used trylock to show the non-blocking variant
//Rest is same for defining mutex structure.
void mutex_lock ( Mutex * mutex )
{
	int n = pthread_mutex_trylock ( mutex );
	if ( n != 0) {
		perror("Could not lock\n");
		exit(-1);
	}
}

void mutex_unlock ( Mutex * mutex )
{
	int n = pthread_mutex_unlock ( mutex );
	if ( n != 0){ 
		perror("Could not unlock");
		exit(-1);
		}
}

//Here Conditional variable is made
typedef pthread_cond_t Cond ;
Cond * make_cond ()
{
	Cond * cond = check_malloc ( sizeof ( Cond ));
	int n = pthread_cond_init ( cond , NULL );
	if (n != 0) perror_exit (" make_cond failed ");
	return cond ;
}

//In conditional variable, for non-blocking variant, I have returned the value of wait so that it can be used in the wait function of semaphore.
int cond_wait ( Cond * cond , Mutex * mutex )
{
	int n = pthread_cond_wait ( cond , mutex );
	//if (n != 0) perror_exit (" cond_wait failed ");
	return n;
}

void cond_signal ( Cond * cond )
{
	int n = pthread_cond_signal ( cond );
	if (n != 0) perror_exit (" cond_signal failed ");
}

//this is the structure for implementing semaphore.
typedef struct {
	int value , wakeups ;
	Mutex * my_semaphore ;
	Cond * cond ;
} Semaphore ;


void make_semaphore ( Semaphore *semaphore, int value )
{
	semaphore -> value = value ;
	semaphore -> wakeups = 0;
	semaphore -> my_semaphore = make_mutex ();
	semaphore -> cond = make_cond ();
}

//in this function, it checks if the cond_wait is successful
//if it is successful, then the mutex is unlocked
void sem_wait ( Semaphore * semaphore )
{
	mutex_lock(semaphore->my_semaphore);
	if ( semaphore -> value < 0) {
		do {
		//if non zero, value, then unlock and increment
		
			int n = cond_wait ( semaphore -> cond , semaphore -> my_semaphore );
			if(n!=0){
				mutex_unlock(semaphore -> my_semaphore);
				semaphore -> value ++;
			}
		} while ( semaphore -> wakeups < 1);
		semaphore -> wakeups --;
	}
	mutex_unlock ( semaphore -> my_semaphore);
}
void sem_post ( Semaphore * semaphore )
{
	//there is no thread to give the signal, therefore the mutex is locked
	//and no if condition is required
	mutex_lock ( semaphore -> my_semaphore);
	semaphore -> value ++;
	// no if condition
	//mutex is unlocked here
	mutex_unlock ( semaphore -> my_semaphore);
}

//this is the function to print the value of semaphore.
void signal (Semaphore * semaphore){
	printf("The value is: %d\n", semaphore->value);
}

int k;

Semaphore room;
Semaphore room1;
Semaphore chopstick[5];

void * philosopher(void *);
void eat(int);

int main()
{
	
	printf("enter the value of k\n");
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

void * philosopher(void * num)
{
while(1){
	int phil=*(int *)num;

	sem_wait(&room);
	sem_wait(&room1);
	//printf("P%d is thinking\n",phil);
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

void eat(int phil)
{
	printf("Philosopher %d eats using forks %d %d\n",phil, phil, (phil+1)%k);
}

