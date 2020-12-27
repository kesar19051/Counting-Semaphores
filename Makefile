all:
	gcc Blocking_2019051.c -o Blocking -lpthread
	gcc NonBlocking_2019051.c -o NonBlocking -lpthread
	
runBlocking:
	./Blocking
	
runNonBlocking:
	./NonBlocking
	
clean:
	rm Blocking
	rm NonBlocking
