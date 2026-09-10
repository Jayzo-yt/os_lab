// process creation ex4

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[]) {
    int n = argc - 1;
    int nums[n];
    for (int i = 0; i < n; i++) nums[i] = atoi(argv[i+1]);

    // create shared memory for 3 ints: average, max, min
    int shmid = shmget(IPC_PRIVATE, 3 * sizeof(int), IPC_CREAT | 0666);
    int *shared = (int*) shmat(shmid, NULL, 0);
    // shared[0] = average, shared[1] = max, shared[2] = min

    pid_t p1 = fork();
    if (p1 == 0) {
        int sum = 0;
        for (int i = 0; i < n; i++) sum += nums[i];
        shared[0] = sum / n;
        shmdt(shared);
        exit(0);
    }

    pid_t p2 = fork();
    if (p2 == 0) {
        int max = nums[0];
        for (int i = 1; i < n; i++) if (nums[i] > max) max = nums[i];
        shared[1] = max;
        shmdt(shared);
        exit(0);
    }

    pid_t p3 = fork();
    if (p3 == 0) {
        int min = nums[0];
        for (int i = 1; i < n; i++) if (nums[i] < min) min = nums[i];
        shared[2] = min;
        shmdt(shared);
        exit(0);
    }

    wait(NULL); wait(NULL); wait(NULL);

    printf("The average value is %d\n", shared[0]);
    printf("The maximum value is %d\n", shared[1]);
    printf("The minimum value is %d\n", shared[2]);

    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
