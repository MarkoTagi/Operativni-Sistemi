#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define NUMS 5

void printArray(int*, int);

int main() {
    int pipefd1[2], pipefd2[2];
    if (pipe(pipefd1) == -1) {
        fprintf(stderr, "First pipe() has failed!\n");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd2) == -2) {
        fprintf(stderr, "Second pipe() has failed");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork() has failed!\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        close(pipefd1[0]);
        close(pipefd2[1]);
        srand(time(0));
        int randomNumbers[NUMS];
        for (int i = 0; i < NUMS; ++i) randomNumbers[i] = rand() % 100;
        printf("Generated: ");
        printArray(randomNumbers, NUMS);
        write(pipefd1[1], randomNumbers, NUMS * sizeof(int));
        close(pipefd1[1]);
        int bytesReceived = read(pipefd2[0], randomNumbers, NUMS * sizeof(int));
        printf("Even numbers: %ld.\n", bytesReceived / sizeof(int));
        printf("[Parent] Exiting...\n");
    }
    else {
        close(pipefd1[1]);
        close(pipefd2[0]);
        int randomNumbers[NUMS];
        read(pipefd1[0], randomNumbers, NUMS * sizeof(int));
        close(pipefd1[0]);
        for (int i = 0; i < NUMS; ++i) if (randomNumbers[i] % 2 == 0) write(pipefd2[1], randomNumbers, 1 * sizeof(int)); 
        close(pipefd2[1]);
        printf("[Child] Exiting...\n");
    }
    return 0;
}

void printArray(int inputArray[], int arrayLength) {
    printf("[ ");
    for (int i = 0; i < (arrayLength - 1); ++i) printf("%d, ", inputArray[i]);
    printf("%d ]\n", inputArray[arrayLength - 1]);
}