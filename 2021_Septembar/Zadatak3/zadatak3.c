#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define ARR_SIZE 1024
#define COUNT 4096
#define MAX_LEN 255

void signalHandler(int);

int main() {
    if (signal(SIGUSR1, &signalHandler) == SIG_ERR) {
        fprintf(stderr, "Error! [SIGUSR1 handler failed]\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGUSR2, &signalHandler) == SIG_ERR) {
        fprintf(stderr, "Error! [SIGUSR2 handler failed]\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error! [fork() has failed]\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        srand(time(0));
        FILE* filep = fopen("prenos.dat", "wb");
        for (int i = 0; i < COUNT; ++i) {
            filep = fopen("prenos.dat", "ab");
            for (int j = 0; j < ARR_SIZE; ++j) {
                int randomNumber = rand() % 99 + 1;
                fprintf(filep, "%d\n", randomNumber);
            }
            fclose(filep);
            kill(pid, SIGUSR1);
            pause();
        }
        kill(pid, SIGUSR1);
        printf("Parent is exiting...\n"); fflush(stdout);
    }
    else {
        pause();
        int offset = 0;
        for (int i = 0; i < COUNT; ++i) {
            FILE* filep = fopen("prenos.dat", "rb");
            fseek(filep, offset, 0);
            char buffer[MAX_LEN];
            int count = 0;
            while (fgets(buffer, MAX_LEN, filep) != NULL) {
                int lineLength = strlen(buffer);
                offset += lineLength;
                buffer[--lineLength] = '\0';
                printf("%d Read [%s]\n", (count++) + 1, buffer);
            }
            fclose(filep);
            printf("\n"); fflush(stdout);
            int ppid = getppid();
            kill(ppid, SIGUSR2);
            pause();
        }
        printf("Child is exiting...\n"); fflush(stdout);
    }
    return 0;
}

void signalHandler(int signalNumber) {
    if (signalNumber == SIGUSR1) {
        printf("Your turn!\n");
    }
    else if (signalNumber == SIGUSR2) {
        printf("I'm done!\n");
    }
}