#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main() {
    int pipe1[2];  // child1 -> parent
    int pipe2[2];  // child2 -> parent

    pipe(pipe1);
    pipe(pipe2);

    srand(time(NULL));
    int target = (rand() % 100) + 1;
    printf("Target number: %d\n", target);

    pid_t c1 = fork();
    if (c1 == 0) {
        // ---- Child 1 ----
        close(pipe1[0]);   // not reading
        close(pipe2[0]);   // not touching pipe2 at all
        close(pipe2[1]);

        srand(time(NULL) ^ getpid());
        int guesses = 0, guess;
        do {
            guess = (rand() % 100) + 1;
            guesses++;
            printf("Child1 guesses %d\n", guess);
            sleep(1);
        } while (guess != target);

        write(pipe1[1], &guesses, sizeof(int));  // report result
        close(pipe1[1]);
        exit(0);
    }

    pid_t c2 = fork();
    if (c2 == 0) {
        // ---- Child 2 ----
        close(pipe2[0]);
        close(pipe1[0]);
        close(pipe1[1]);

        srand(time(NULL) ^ getpid());
        int guesses = 0, guess;
        do {
            guess = (rand() % 100) + 1;
            guesses++;
            printf("Child2 guesses %d\n", guess);
            sleep(1);
        } while (guess != target);

        write(pipe2[1], &guesses, sizeof(int));
        close(pipe2[1]);
        exit(0);
    }

    // ---- Parent ----
    close(pipe1[1]);
    close(pipe2[1]);

    int g1, g2;
    read(pipe1[0], &g1, sizeof(int));   // blocks until child1 finishes
    read(pipe2[0], &g2, sizeof(int));   // blocks until child2 finishes
    close(pipe1[0]);
    close(pipe2[0]);

    printf("Child1 took %d guesses\n", g1);
    printf("Child2 took %d guesses\n", g2);

    if (g1 < g2) printf("Child1 wins!\n");
    else if (g2 < g1) printf("Child2 wins!\n");
    else printf("It's a tie!\n");

    wait(NULL);
    wait(NULL);
    return 0;
}
