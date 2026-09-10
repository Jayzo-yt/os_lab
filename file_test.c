// process creation ex3

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define FILENAME "info.txt"

void read_odd_lines() {
    FILE *fp = fopen(FILENAME, "r");
    char line[256];
    int lineno = 0;
    while (fgets(line, sizeof(line), fp)) {
        lineno++;
        if (lineno % 2 != 0) {
            printf("[C1 - odd line %d]: %s", lineno, line);
        }
    }
    fclose(fp);
}

void read_even_lines() {
    FILE *fp = fopen(FILENAME, "r");
    char line[256];
    int lineno = 0;
    while (fgets(line, sizeof(line), fp)) {
        lineno++;
        if (lineno % 2 == 0) {
            printf("[C2 - even line %d]: %s", lineno, line);
        }
    }
    fclose(fp);
}

int main() {
    pid_t c1 = fork();
    if (c1 == 0) {
        read_odd_lines();
        exit(0);
    }

    pid_t c2 = fork();
    if (c2 == 0) {
        read_even_lines();
        exit(0);
    }

    // parent waits for BOTH children to finish reading first
    wait(NULL);
    wait(NULL);

    // now parent appends random integers
    FILE *fp = fopen(FILENAME, "a");
    srand(time(NULL));
    for (int i = 0; i < 3; i++) {
        fprintf(fp, "%d\n", rand() % 1000);
    }
    fclose(fp);

    printf("Parent appended random integers.\n");
    return 0;
}
