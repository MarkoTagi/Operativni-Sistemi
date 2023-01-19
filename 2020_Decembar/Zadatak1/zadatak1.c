#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define MAX_LEN 21

int main() {
    int pipefd1[2], pipefd2[2];
    if (pipe(pipefd1) == -1) {
        fprintf(stderr, "First pipe() has failed!\n");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd2) == -1) {
        fprintf(stderr, "Second pipe() has failed!\n");
        exit(EXIT_FAILURE);
    }
    int pid1 = fork();
    if (pid1 < 0) {
        fprintf(stderr, "First fork() has failed!\n");
        exit(EXIT_FAILURE);
    }
    if (pid1 > 0) {
        int pid2 = fork();
        if (pid2 < 0) {
            fprintf(stderr, "Second fork() has failed!\n");
            exit(EXIT_FAILURE);
        }
        if (pid2 > 0) { // Process1
            close(pipefd1[0]);
            close(pipefd2[0]);
            srand(time(0));
            char randomChars[MAX_LEN];
            int asciiValue = 0;
            for (int i = 0; i < MAX_LEN; ++i) {
                randomChars[i] = (rand() % 95) + 32;
                asciiValue += randomChars[i];
            }
            randomChars[MAX_LEN - 1] = '\0';
            printf("Generated: [%s]\n", randomChars);
            printf("ASCII Value: %d.\n", asciiValue);
            if (asciiValue % 2 == 0) write(pipefd1[1], randomChars, MAX_LEN);
            else write(pipefd2[1], randomChars, MAX_LEN);
            close(pipefd1[1]);
            close(pipefd2[1]);
            printf("[Process1] Exiting...\n");
        }
        else { // Process2
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);
            char randomChars[MAX_LEN];
            int bytesReceived = read(pipefd1[0], randomChars, MAX_LEN);
            if (bytesReceived > 0) {
                FILE* filep = fopen("KARAKTERI.txt", "w");
                fputs(randomChars, filep);
                fclose(filep);
            }
            close(pipefd1[0]);
            printf("[Process2] Exiting...\n");
        }
    }
    else { //Process3
        close(pipefd1[0]);
        close(pipefd1[1]);
        close(pipefd2[1]);
        char randomChars[MAX_LEN];
        int bytesReceived = read(pipefd2[0], randomChars, MAX_LEN);
        if (bytesReceived > 0) printf("Received: [%s]\n", randomChars);
        close(pipefd2[0]);
        printf("[Process3] Exiting...\n");
    }
    return 0;
}