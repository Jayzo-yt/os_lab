//number hunter process creation ex2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main() {
    srand(time(NULL));
    int target = (rand() % 100) + 1;
    printf("Target number: %d\n", target);

    char target_str[10];
    sprintf(target_str, "%d", target);

    for (int player = 1; player <= 2; player++) {
        pid_t pid = fork();

        if (pid == 0) {
            // child
            char id_str[10];
            sprintf(id_str, "%d", player);
            execl("./hunter", "hunter", target_str, id_str, NULL);
            perror("execl failed");
            exit(1);
        }
        // parent loops again to fork the next child
    }

    int status;
    pid_t winner_pid = -1;
    for (int i = 0; i < 2; i++) {
        pid_t child_pid = wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
            winner_pid = child_pid;
        }
    }

    if (winner_pid != -1)
        printf("Winning process PID: %d\n", winner_pid);

    return 0;
}
