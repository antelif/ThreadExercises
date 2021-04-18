

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

const int TURNS = 3; // how many turns will be played for each player.
/*

	This program prints each player's turn.
	Main thread is player 1.
	Thread is player 2.

	Program arguments:
	1. How many seconds will it take for player 1 to play.
	2. How many seconds will it take for palyer 2 to play.

	It is interesting to see here how threads work.
	The bigger the difference of sleep seconds the bigger the probability a player
	will take all their turns at once!
	The smaller the difference the bigger the probability turns
	will be intervchangable.
	Give it a try!
*/
void *playerTurn(void *threadId) {

	// Cast argument to integer
	int seconds = *((int *)threadId);

	for(int i = 0; i < TURNS; i++ ){

		/* Tells thread to go to sleep for 'seconds' seconds. In the meantime othaer
		** threads will work.
		** Meaning that the other player will not take turn!
		*/
		sleep(seconds);

		printf("Player 2: Playing...\n" );
	}
	return NULL;
}

void mainTurn(int *seconds){

	for(int i = 0; i < TURNS; i++){

		/* Tells thread to go to sleep for 'seconds' seconds. In the meantime othaer
		** threads will work.
		** Meaning that the other player will not take turn!
		*/
		sleep(*seconds);
		printf("Player 1: Playing...\n");

	}
}

int main(int argc, char *argv[]) {

	if(argc != 3){
		printf("ERROR: Invalid number of arguments.\n");
		exit(-1);
	}

	// Seconds each player plays.
	int playerOneSeconds = atoi(argv[1]);
	int playerTwoSeconds = atoi(argv[2]);

	// Thread of player 2.
	pthread_t playerTwoThread;
	pthread_create(&playerTwoThread, NULL, &playerTurn, &playerTwoSeconds);

	// Join main() and playerTwoThread, otherwise main will start and finish
	// before playerTwoThread.
	// Player 1 aka main() thread takes turn first.
	mainTurn(&playerOneSeconds);
	pthread_join(playerTwoThread, NULL);

	return 1;
}
