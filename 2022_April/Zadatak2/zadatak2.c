#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

void printArray(int inputArray[], int arrayLength) {
    printf("[ ");
    for (int i = 0; i < (arrayLength - 1); ++i) printf("%d, ", inputArray[i]);
    printf("%d ]\n", inputArray[arrayLength - 1]);
}

int main() {
    int pipefd1[2], pipefd2[2];
    if (pipe(pipefd1) == -1) {
        fprintf(stderr, "pipe(pipefd1) has failed!\nThe process will now exit...\n");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd2) == -1) {
        fprintf(stderr, "pipe(pipefd2) has failed!\nThe process will now exit...\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork() has failed!\nThe process will now exit...\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        close(pipefd1[0]);
        close(pipefd2[1]);
        srand(time(0));
        int randomNumbers[10];
        for (int i = 0; i < 10; ++i) randomNumbers[i] = (rand() % 100) + 200;
        printf("Generated: ");
        printArray(randomNumbers, 10);
        write(pipefd1[1], randomNumbers, 10 * sizeof(int));
        close(pipefd1[1]);
        wait(NULL);
        int bytesReceived = read(pipefd2[0], randomNumbers, 10 * sizeof(int));
        //printf("Numbers received: %ld.\n", bytesReceived / sizeof(int));
        printf("Received: ");
        printArray(randomNumbers, bytesReceived / sizeof(int));
        close(pipefd2[0]);
        printf("Parent> Exiting...\n");
    }
    else {
        close(pipefd1[1]);
        close(pipefd2[0]);
        int randomNumbers[10];
        read(pipefd1[0], randomNumbers, 10*(sizeof(int)));
        close(pipefd1[0]);
        int count = 0;
        for(int i = 0; i < 10; ++i) {
            if (randomNumbers[i] % 3 == 0) {
                //printf("Divisible by 3 (%d)!\n", randomNumbers[i]);
                randomNumbers[i] += 25;
                ++count;
                if (i != (count - 1)) {
                    int tmp = randomNumbers[count - 1];
                    randomNumbers[count - 1] = randomNumbers[i];
                    randomNumbers[i] = tmp;
                }
            }
        }
        //printArray(randomNumbers, count);
        write(pipefd2[1], randomNumbers, count * sizeof(int));
        close(pipefd2[1]);
        printf("Child> Exiting...\n");
    }
    return 0;
}