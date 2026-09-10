//number hunter hunter.c process creation 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int target = atoi(argv[1]);
    int player_id = atoi(argv[2]);

    srand(time(NULL) ^ getpid());  // different seed per process — IMPORTANT

    int guesses = 0;
    int guess;

    do {
        guess = (rand() % 100) + 1;
        guesses++;
        printf("Player %d guesses %d\n", player_id, guess);
        sleep(1);
    } while (guess != target);

    printf("Player %d WON with %d guesses!\n", player_id, guesses);
    exit(1);  // exit status 1 = winner
}
