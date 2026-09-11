#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#define N 100

int main() {
    // shared memory holds: p (the number) and turn (whose turn it is)
    int shmid = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    int *shared = (int*) shmat(shmid, NULL, 0);

    int *p = &shared[0];
    int *turn = &shared[1];

    *p = 1;
    *turn = 1;   // 1 = John's turn, 2 = Michael's turn

    pid_t pid = fork();

    if (pid == 0) {
        // ---- Michael (child) ----
        srand(time(NULL) ^ getpid());
        while (1) {
            while (*turn != 2) { }        // busy-wait until it's Michael's turn

            if (*p >= N) break;

            int r = (rand() % 8) + 2;
            *p = *p * r;
            printf("Michael: x%d -> p=%d\n", r, *p);

            if (*p >= N) {
                printf("Michael wins!\n");
                *turn = 1;
                break;
            }
            *turn = 1;   // hand turn back to John
        }
        exit(0);
    }
    else {
        // ---- John (parent) ----
        srand(time(NULL));
        while (1) {
            while (*turn != 1) { }        // busy-wait until it's John's turn

            if (*p >= N) break;

            int r = (rand() % 8) + 2;
            *p = *p * r;
            printf("John: x%d -> p=%d\n", r, *p);

            if (*p >= N) {
                printf("John wins!\n");
                *turn = 2;
                break;
            }
            *turn = 2;   // hand turn back to Michael
        }
        wait(NULL);
    }

    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
