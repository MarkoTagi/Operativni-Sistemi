#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* countingThread(void* arg) {
    int count = *((int*)arg);
    printf("[IN COUNTING_THREAD]\n\tCounter: " );
    for (int i = 0; i < count; ++i) {
        printf("%d...", i);
        fflush(stdout);
        sleep(3);
    }
    printf("%d.\n", count);
}

int main() {
    pthread_t countingThread_id;
    int userInput;
    do {
        printf("[IN MAIN] \n\tEnter a positive integer (or \"99\" to exit): ");
        scanf("%d", &userInput);
        if (userInput <= 0) printf("\tYou've entered a non-positive number.\n");
        else if (userInput != 99) {
            pthread_create(&countingThread_id, NULL, countingThread, &userInput);
            pthread_join(countingThread_id, NULL);
        }
    } while (userInput != 99);
    printf("\tExiting...\n");
    return 0;
}
