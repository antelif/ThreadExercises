/**
 ** This program created 5 threads that increment a counter to 5.
 ** Threads increment a counter.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const int THREADS = 10;

// Create pthread_mutex_t to lock pthread_t
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// The counter threads increment
int counter;
// Status of varius operations
int statusId;

// Method to be called by threads. Increment counter.
void *increaseCounter(void *threadId) {
	// Cast void to int
	int *tid = (int *)threadId;
	int prevValue;

	// Lock counter incremetation.
	statusId = pthread_mutex_lock(&lock);
	if(statusId != 0){
		printf("ERROR: Mutex lock failed\n");
	}

	prevValue = counter;

	// Increment counter
	counter++;

	// Sleep a little so messages print delayed :)
	sleep(1);

	printf("Thread %d: Previous value = %d, New value = %d.\n", *tid, prevValue, counter);

	// Unlock
	statusId = pthread_mutex_unlock(&lock);
	if(statusId != 0){
		printf("ERROR: Mutex unlock failed\n");
	}

	pthread_exit(threadId);
}

int main(int argc, char *argv[]) {

	// Allocate memory for threads
	pthread_t *threads;
	threads = malloc(THREADS*sizeof(pthread_t));
	if(threads == NULL){
		printf("ERROR: Could not allocate memory for threads.\n");
	}

	// Allocate memory for thread ids.
	int *threadIds;
	threadIds = malloc(THREADS*sizeof(int));
	if(threadIds == NULL){
		printf("ERROR: Could not allocate memory for thread id.\n");
	}

	for (int i = 0; i < THREADS; i++) {
		threadIds[i] = i + 1;
		printf("Main: creating thread %d\n", threadIds[i]);

		// Create thread
    int  threadId = pthread_create(&threads[i], NULL, increaseCounter, &threadIds[i]);
		if (threadId != 0) {
			printf("ERROR: return code from pthread_create() is %d\n", threadId);
   			exit(-1);
   		}
	}

	void *status;
	for (int i = 0; i < THREADS; i++) {
		int threadId = pthread_join(threads[i], &status);

		if (threadId != 0) {
			printf("ERROR: return code from pthread_join() is %d\n", threadId);
			exit(-1);
		}

		printf("Main: Thread %d finished with status %d.\n", threadIds[i], *(int *)status);
	}

	printf("Counter value after threads finished = %d.\n", counter);

// Free allocated space
	free(threads);
	free(threadIds);
	// Destroy Mutex
	statusId = pthread_mutex_destroy(&lock);
	if(statusId != 0){
		printf("ERROR: Failed to destroy mutex\n");
	}
	return 1;
}
