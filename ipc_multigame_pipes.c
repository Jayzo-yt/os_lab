#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define N 100

int main() {
    int p1[2];  // John -> Michael
    int p2[2];  // Michael -> John

    pipe(p1);
    pipe(p2);

    pid_t pid = fork();

    if (pid == 0) {
        // ---- Michael (child) ----
        close(p1[1]);   // not writing to p1
        close(p2[0]);   // not reading from p2
        srand(time(NULL) ^ getpid());

        int p;
        while (1) {
            read(p1[0], &p, sizeof(int));      // wait for John's number
            if (p >= N) break;

            int r = (rand() % 8) + 2;
            p = p * r;
            printf("Michael: x%d -> p=%d\n", r, p);
            if (p >= N) {
                printf("Michael wins!\n");
                write(p2[1], &p, sizeof(int));
                break;
            }
            write(p2[1], &p, sizeof(int));
        }
        close(p1[0]);
        close(p2[1]);
        exit(0);
    }
    else {
        // ---- John (parent) ----
        close(p1[0]);   // not reading from p1
        close(p2[1]);   // not writing to p2
        srand(time(NULL));

        int p = 1;
        write(p1[1], &p, sizeof(int));   // start the game

        while (1) {
            read(p2[0], &p, sizeof(int));      // wait for Michael's number
            if (p >= N) break;

            int r = (rand() % 8) + 2;
            p = p * r;
            printf("John: x%d -> p=%d\n", r, p);
            if (p >= N) {
                printf("John wins!\n");
                write(p1[1], &p, sizeof(int));
                break;
            }
            write(p1[1], &p, sizeof(int));
        }

        printf("Final p = %d. ", p);
        printf("%s wins!\n", (p == p) ? "Someone" : "??");  // see note below

        close(p1[1]);
        close(p2[0]);
        wait(NULL);
    }
    return 0;
}
