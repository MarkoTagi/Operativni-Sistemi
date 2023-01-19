#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <math.h>
#define MAX_LEN 10

sem_t write_sem;
sem_t read_sem;

int stringToInteger(const char* string) {
    int stringLength = strlen(string);
    int theNumber = 0;
    for (int i = 0; i < stringLength; ++i) {
        int notDigit = string[i] < '0' || string[i] > '9';
        if (notDigit && (string[i] != '-' || i != 0)) return -1;
        int aDigit = (string[i] == '-') ? -(string[++i] - '0') : string[i] - '0';
        theNumber += (theNumber >= 0) ? aDigit * pow(10, stringLength - 1 - i) : -(aDigit * pow(10, stringLength - 1 - i));
    }
    return theNumber;
}

void* countingThread(void* arg) {
    sem_wait(&read_sem);
    int counter;
    while (1) {
        counter = *((int*)arg);
        printf("Counter: ", counter);
        for (int i = 0; i < counter; ++i) {
            printf("%d...", counter - i);
            fflush(stdout);
            sleep(3);
        }
        printf("0!\n");
        sem_post(&write_sem);
        sem_wait(&read_sem);
    }
}

int main() {
    char userInput[MAX_LEN];
    int counter;
    sem_init(&write_sem, 0, 1);
    sem_init(&read_sem, 0, 0);
    pthread_t countingThread_id;
    pthread_create(&countingThread_id, NULL, countingThread, &counter);
    do {
        sem_wait(&write_sem);
        printf("Enter a positive integer (or type \"END\" to exit): ");
        fgets(userInput, MAX_LEN, stdin);
        int inputLength = strlen(userInput);
        userInput[inputLength - 1] = '\0';
        if (strcmp(userInput, "END") != 0) {
            counter = stringToInteger(userInput);
            if (counter > 0) sem_post(&read_sem);
            else {
                printf("Invalid input (%d)!\n", counter);
                sem_post(&write_sem);
            }
        }
    } while (strcmp(userInput, "END") != 0);
    sem_destroy(&write_sem);
    sem_destroy(&read_sem);
    return 0;
}