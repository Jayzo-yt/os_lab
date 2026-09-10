#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

#define N 100

int main() {
    // 1. Create shared memory to hold p
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    int *p = (int*) shmat(shmid, NULL, 0);
    *p = 1;

    // 2. Create two semaphores: john_turn starts at 1 (goes first), michael_turn starts at 0
    sem_t *john_turn = sem_open("/john_turn", O_CREAT, 0666, 1);
    sem_t *michael_turn = sem_open("/michael_turn", O_CREAT, 0666, 0);

    pid_t pid = fork();

    if (pid == 0) {
        // ---- Michael (child) ----
        srand(time(NULL) ^ getpid());
        while (1) {
            sem_wait(michael_turn);          // wait for my turn
            if (*p >= N) { sem_post(john_turn); break; }

            int r = (rand() % 8) + 2;
            *p = *p * r;
            printf("Michael: x%d -> p=%d\n", r, *p);

            if (*p >= N) {
                printf("Michael wins!\n");
                sem_post(john_turn);
                break;
            }
            sem_post(john_turn);             // hand turn back to John
        }
        exit(0);
    }
    else {
        // ---- John (parent) ----
        srand(time(NULL));
        while (1) {
            sem_wait(john_turn);
            if (*p >= N) break;

            int r = (rand() % 8) + 2;
            *p = *p * r;
            printf("John: x%d -> p=%d\n", r, *p);

            if (*p >= N) {
                printf("John wins!\n");
                sem_post(michael_turn);
                break;
            }
            sem_post(michael_turn);
        }
        wait(NULL);
    }

    // cleanup
    sem_close(john_turn);
    sem_close(michael_turn);
    sem_unlink("/john_turn");
    sem_unlink("/michael_turn");
    shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
