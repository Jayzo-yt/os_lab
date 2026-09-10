//process creation ex1

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char input[100];
    char *args[10];

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = 0;  // strip newline

        if (strlen(input) == 0) continue;
        if (strcmp(input, "exit") == 0) break;

        // parse input into args[]
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;   // execvp REQUIRES a NULL-terminated array

        pid_t pid = fork();

        if (pid < 0) {
            printf("Fork failed\n");
        }
        else if (pid == 0) {
            // child
            execvp(args[0], args);
            printf("Command not found: %s\n", args[0]);
            exit(1);   // if execvp fails, kill this child — otherwise it becomes a second shell!
        }
        else {
            // parent
            wait(NULL);
        }
    }
    return 0;
}
