/**

	This program creates multiple threads.
	Each thread prints a message with its id.

	Arguments: Number of threads to be created.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Prints the message of the thread.
void *PrintMessage(void *threadId) {

	int* id = (int*)threadId;

	printf("Thread: %d\n", *id );

	pthread_exit(id);
}


int main(int argc, char *argv[]) {

	if(argc != 2){

		printf("ERROR: Invalid number of arguments.\n");

		exit(-1);

	}else if( argv[1] < 0){

		printf("ERROR: Invalid argument.\n");

		exit(-1);
	}

	int maxNumberOfThreads = atoi(argv[1]);


	printf("Main: We will create %d for threads that will print hello world.\n", maxNumberOfThreads);

	// Create a thread pointer: points to a 'sequence' or 'array' of threads.
   	pthread_t *threads;

   	// Allocate memory space and check if allocation was succesful.
   	threads = malloc(maxNumberOfThreads * sizeof(pthread_t));

   	if(threads == NULL){
   		printf("ERROR: Memory allocation failed\n\n");
   		return(-1);
   	}

   	// Stores thread ids
   	int threadId[maxNumberOfThreads];

   	for(int i = 0; i < maxNumberOfThreads; i++){

   		printf("Main: Creating thread with id: %d\n", i+1);

   		threadId[i] = i + 1;

   		int id = pthread_create(&threads[i], NULL, PrintMessage, &threadId[i]);

   		// id should be 0
   		if(id != 0){
   			printf("ERROR: Return code of pthread_create() is %d\n",id);
   			exit(-1);
   		}
   	}

   	// Always join threads - Children threads should exit before parent threads.
   	void *status;

   	for (int i = 0; i < maxNumberOfThreads; i++){

   		int id = pthread_join(threads[i], &status);

   		if(id !=0){
   			printf("ERROR: Return code from pthread_join() is %d\n", id);
   			exit(-1);
   		}
   		printf("Main: Thread %d - status %d\n", threadId[i], (*(int*)status));
   	}

   	// Free allocated memory
   	free(threads);

	return 1;
}
