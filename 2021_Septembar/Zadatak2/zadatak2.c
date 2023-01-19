#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

void printArray(int inputArray[], int arrayLength) {
    printf("[ ");
    for (int i = 0; i < arrayLength - 1; ++i) printf("%d, ", inputArray[i]);
    printf("%d ]\n", inputArray[arrayLength - 1]);
}

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "pipe() has failed!\nThe process will now exit...\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork() has failed!\nThe process will now exit...\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        close(pipefd[0]);
        srand(time(0));
        int randomNumbers[10];
        for (int i = 0; i < 10; ++i) randomNumbers[i] = rand() % 100;
        printf("Generated: ");
        printArray(randomNumbers, 10);
        write(pipefd[1], randomNumbers, 10 * sizeof(int));
        close(pipefd[1]);
    }
    else {
        close(pipefd[1]);
        int randomNumbers[10];
        read(pipefd[0], randomNumbers, 10 * sizeof(int));
        for (int i = 0; i < 10; ++i) {
            if ((randomNumbers[i] % 2) == 0) printf("Even number! (%d)\n", randomNumbers[i]);
            else printf("%d\n", randomNumbers[i]);
        }
        close(pipefd[0]);
    }
    return 0;
}