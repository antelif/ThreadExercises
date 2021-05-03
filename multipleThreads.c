/**
** This program creates NUM_THREADS threads.
** Thread 0 - NUM_THREADS-1: Increment a counter.
** Thread NUM_THREADS: If counter is equal to a constant, doubles the counter.
** Threads 0 to NUM_THREADS-1 should run TCOUNT times each.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 4
#define TCOUNT 2
#define COUNTER_LIMIT 5

// Counter to be increased and doubled.
int counter = 0;
// 1 if thread that doubles the counter has started.
int doubleThreadStarted = 0;
// 1 if thread that doubles the counter has finished.
int doubleThreadDone = 0;
// 0 if no signal has been sent to double to awake if cunter has reached COUNT_LIMIT
int signalSent = 0;

pthread_mutex_t counterMutex;
pthread_cond_t counterCondition;


// Method to be called by threads that increment count variable.
void *increaseCounter(void *t) {
	int rc;
	int *threadId = (int *)t;

	// Lock 1 --------------------------
	rc = pthread_mutex_lock(&counterMutex);
	if(rc != 0){
		printf("ERROR: Mutex lock failed with code %d.\n", rc);
		pthread_exit(&rc);
	}

	while(doubleThreadStarted == 0){
		printf("Increase - Thread %d will wait for signal from doubleCounter().\n", *threadId);
		rc = pthread_cond_wait(&counterCondition, &counterMutex);
		if(rc != 0){
			printf("ERROR: Condition wait failed with code %d.\n", rc);
			pthread_exit(&rc);
		}

		printf("Increase - Thread %d has woken up.\n", *threadId);
	}
	// Unlock 1 ---------------------------
	rc = pthread_mutex_unlock(&counterMutex);
	if(rc != 0){
		printf("ERROR: Mutex unlock failed with code %d.\n", rc);
		pthread_exit(&rc);
	}

	// Do TCOUNT iterations and increase counter.
	for (int i = 0; i < TCOUNT; i++) {

		// Lock mutex and check lock succeeded.
		rc = pthread_mutex_lock(&counterMutex);
		if(rc != 0){
			printf("ERROR: Mutex lock failed with code: %d\n", rc);
			pthread_exit(&rc);
		}

		// Signal if COUNTER_LIMIT reached and no signal has been sent yet.
		if(counter == COUNTER_LIMIT){

			if( signalSent == 0){
				// Note that signal waits just one thread - the one with the highest priority
				// Broadcast wakes all threads, therefore used in doubleCounter().
				rc = pthread_cond_signal(&counterCondition);
				if(rc != 0){
					printf("ERROR: Condition signal failed with code: %d\n", rc);
					pthread_exit(&rc);
				}
				signalSent = 1;
			}

			// Wait until doubleCounter() doubles the counter.
			while(doubleThreadDone == 0){
				printf("Increase - Thread %d will wait for counter to be doubled...\n", *threadId);
				rc = pthread_cond_wait(&counterCondition, &counterMutex);
				if(rc != 0){
					printf("ERROR: Condition wait failed with code: %d\n", rc);
					pthread_exit(&rc);
				}
			}
		}

		// Increase counter.
		counter++;
		printf("Increase - Thread %d counter = %d\n", *threadId, counter);

		rc = pthread_mutex_unlock(&counterMutex);
		if(rc != 0){
			printf("ERROR: Mutex unlock failed with code: %d\n", rc);
			pthread_exit(&rc);
		}
	}
	// If threads that increase counter complete iterations and thread that doubles counter
	// is not yet called
	// Case (NUM_THREADS-1)*TCOUNT == COUNTER_LIMIT
	if( signalSent == 0){
		// Note that signal waits just one thread - the one with the highest priority
		// Broadcast wakes all threads, therefore used in doubleCounter().
		rc = pthread_cond_signal(&counterCondition);
		if(rc != 0){
			printf("ERROR: Condition signal failed with code: %d\n", rc);
			pthread_exit(&rc);
		}
		signalSent = 1;
	}
	pthread_exit(t);
}

// Method to be called by threads that double count variable.
void *doubleCounter(void *t) {

	int *threadId = (int *) t;
	printf("Double - Thread %d has started.\n", *threadId);

	// -- Lock 1 -----------------------------
	int rc = pthread_mutex_lock(&counterMutex);
	if(rc != 0){
		printf("ERROR: Mutex lock failed with code %d.\n", rc);
		pthread_exit(&rc);
	}

	// If counter has not reached COUNT_LIMIT yet, send signal and wait.
	if (counter < COUNTER_LIMIT) {
		printf("Double - Thread %d will send signal for other threads to start increasing counter...\n", *threadId);

		// Change variable, since double has started.
		doubleThreadStarted = 1;

		// Signal other threads that it has started.
		rc = pthread_cond_broadcast(&counterCondition);
		if (rc != 0){
			printf("ERROR: Signal from thread %d failed with code: %d\n", *threadId, rc);
			pthread_exit(&rc);
		}
	}

	// Now wait - while is necessarry to avoid spurious wake up.
	while( signalSent == 0){

		printf("Double - Thread %d will wait until counter = %d...\n", *threadId, COUNTER_LIMIT);

		rc = pthread_cond_wait(&counterCondition, &counterMutex);
		if (rc != 0){
			printf("ERROR: Cond from Thread %d failed with code: %d\n",*threadId, rc);
			pthread_exit(&rc);
		}

		printf("Double - Thread %d has woken up. The counter will be doubled.\n", *threadId);
	}

	// Now that has woken up, double the counter.
	counter *= 2;
	printf("Double - Thread %d: counter = %d.\n",*threadId, counter);

	// Work is done.
	doubleThreadDone = 1;

	// Signal threads that the counter has been doubled.
	rc = pthread_cond_broadcast(&counterCondition);
	if (rc != 0){
		printf("ERROR: Signal from thread %d failed with code: %d\n", *threadId, rc);
		pthread_exit(&rc);
	}

	// Unlock mutex and exit
	rc = pthread_mutex_unlock(&counterMutex);
	if(rc != 0){
		printf("ERROR: Mutex unlock lock failed with code %d.\n", rc);
		pthread_exit(&rc);
	}

	pthread_exit(t);
}

int main(int argc, char *argv[]) {

	int i, rc;
	// Allocate memory for threads
	pthread_t *threads;
	threads = malloc(NUM_THREADS*sizeof(pthread_t));
	if(threads == NULL){
		printf("ERROR: Failed to allocate memory for threads.\n");
	}

	// Allocate memory for thread ids.
	int *threadIds;
	threadIds = malloc(NUM_THREADS * sizeof(int));
	if(threadIds == NULL){
			printf("ERROR: Failed to allocate memory for thread ids.\n");
	}

	// Create threads.
	for (int i = 0; i < NUM_THREADS; i++){
		// Create ids for threads
		threadIds[i] = i+1;

		if(i == NUM_THREADS - 1){
			sleep(2);
			pthread_create(&threads[i], NULL, doubleCounter, &threadIds[i]);
		}else{
			pthread_create(&threads[i], NULL, increaseCounter, &threadIds[i]);
		}
	}

	// Join threads
	for (i = 0; i < NUM_THREADS; i++) {
  	int status = pthread_join(threads[i], NULL);
		if(status != 0){
				printf("ERROR: Failed to join threads.\n");
		}
	}

	printf ("Main(): Waited on %d threads. Final value of count = %d.\nExpected value = %d\n",
          NUM_THREADS, counter, expectedValue());

	// Cleanup
	free(threads);
	free(threadIds);

  return 1;
}

int expectedValue(){
	// TCOUNT = 12
	// COUNTER_LIMIT = 11
	// NUM_THREADS = 4
	// Threads that increased counter before doubled.
	int iterations = (NUM_THREADS-1)*TCOUNT;
	int iterationsLeft = iterations;
	int sum = 0;

	if(TCOUNT < COUNTER_LIMIT){
		iterationsLeft = iterations-COUNTER_LIMIT;
	}else{
		iterationsLeft = iterations - COUNTER_LIMIT;
	}
	sum = COUNTER_LIMIT*2+iterationsLeft;
	return sum;
}
