/**
** This program creates a thread that calculates the product of two random
** numbers.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Method to be called by thread. Calculates product.
void *multiplyNumbers(void *args) {

	// Cast void to int
	int *numbers = (int*)args;
	// Get first number
	int *firstNumPtr = numbers;		// Alternatively numbers[0];
	// Get second number
	int *secondNumPtr = numbers+1;// Alternatively numbers[1];

	int sum = (*firstNumPtr) * (*secondNumPtr);

	printf("Thread: %d x %d = %d\n",*firstNumPtr, *secondNumPtr, sum);
}

int main(int argc, char *argv[]) {

	// Allocate memory space to store numbers
	int *numbers;
	numbers = malloc(2 * sizeof(int));
	if(numbers == NULL){
		printf("ERROR: Memory could not be allocated.\n");
		exit(-1);
	}

	// Create random numbers
	for(int i = 0; i < 2; i++){
		srand(i*8167);
		numbers[i] = rand()%100+1;
		printf("Main: Number %d: %d\n",i, numbers[i]);
	}

	// Create thread
	pthread_t thread;
	int threadId = pthread_create(&thread, NULL, multiplyNumbers, numbers);
	if(threadId != 0){
		printf("ERROR: Could not create thread\n");
		exit(-1);
	}

	// Join thread with main()
	void *status;
	int joinId = pthread_join(thread, &status);
	if(joinId != 0){
		printf("ERROR: Could not join threads\n");
		exit(-1);
	}

	// Free allocated space.
	free(numbers);

	return 1;
}
