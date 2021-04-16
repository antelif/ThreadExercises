/**
	Skopos tis askisis einai i ylopoiisi enos programmatos to opoio tha 
	dimiourgei pollapla
	threads ta opoia tha ektypwnoun sto termatiko ena mynima tis morfis Hello 		World from thread threadId,
	opou threadId tha einai i seira dimiourgias tou thread. Gia paradeigma to 		prwto thread tha prepei na ektypwsei to mhynyma: 
	Thread 1: Hello World from thread 1.
    	To plithos twn threads tha to diavazete
    	apo ti grammi entolwn. 
	Aparaitites synartiseis tis pthread.h vivliothikis:
	int pthread_create(pthread_t *thread, const pthread_attr_t *attr, 
		void *(*start_routine)(void*), void *arg);
	void pthread_exit(void *value_ptr);
	int pthread_join(pthread_t thread, void **value_ptr);
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * H synartisi pou tha kaleite apo ta threads gia na typwnei to mynima 
 * Thread: Hello World from thread threadId
*/
void *PrintHello(void *threadId) {
	//ylopoiisi tis ektypwsis pou theloume
	int tid = (*(int*) threadId);
	printf("I am thread %d\n", tid );
	return 0;
}

/**
 * H main synartisi tha pairnei san orisma to plithos twn threads pou prepei na 
 * dimiourgithoun.
*/
int main(int argc, char *argv[]) {

	if(argc != 1){
		printf("ERROR: Invalid number of arguments.\n");
		exit(-1);
	}

	int numberOfThreads = 3;

	int threadId[numberOfThreads];

	for(int i = 0; i < numberOfThreads; i++){

		threadId[i] = i+1;
	}
	
	pthread_t * threads;	

	printf("Main: We will create %d for threads that will print their id.\n", numberOfThreads);
   	

	pthread_create(&threads[0], NULL, PrintHello, &threadId[0]);
	printf("Created thread 1\n");
	pthread_create(&threads[1], NULL, PrintHello, &threadId[1]);
	printf("Created thread 2\n");
	pthread_create(&threads[2], NULL, PrintHello, &threadId[2]);
	printf("Created thread 3\n");
	
	pthread_join(threads[0],NULL);
	pthread_join(threads[1],NULL);
	pthread_join(threads[2],NULL);

	return 1;
}
