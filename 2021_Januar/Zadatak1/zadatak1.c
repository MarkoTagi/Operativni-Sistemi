#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define MAX_LEN 10

int stringToInteger(const char*);
void* countingThread(void*);

int main() {
    char userInput[MAX_LEN];
    do {
        printf("Enter something (or type \"END\" to exit): ");
        fgets(userInput, MAX_LEN, stdin);
        int inputLength = strlen(userInput);
        userInput[inputLength - 1] = '\0';
        if (strcmp(userInput, "END") != 0) {
            int counter = stringToInteger(userInput);
            if (counter == 0) printf("Invalid input! (%s)\n", userInput);
            else {
                pthread_t countingThread_id;
                pthread_create(&countingThread_id, NULL, countingThread, &counter);
                pthread_join(countingThread_id, NULL);
            }
        }
    } while (strcmp(userInput, "END") != 0);
    printf("Exiting...\n");
    return 0;
}

int stringToInteger(const char* string) {
    int stringLength = strlen(string);
    int theNumber = 0;
    for (int i = 0; i < stringLength; ++i) {
        int notDigit = string[i] < '0' || string[i] > '9';
        if (notDigit && (string[i] != '-' || i != 0)) return 0;
        int aDigit = (string[i] == '-') ? -(string[++i] - '0') : string[i] - '0';
        theNumber += (theNumber >= 0) ? aDigit * pow(10, stringLength - 1 - i) : -(aDigit * pow(10, stringLength - 1 - i));
    }
    return theNumber;
}

void* countingThread(void* arg) {
    int counter = *(int*)arg;
    printf("Counter: ");
    for (int i = 0; i < abs(counter); ++i) {
        counter > 0 ? printf("%d...", counter - i) : printf("%d...", counter + i);
        fflush(stdout);
        sleep(2);
    }
    printf("0!\n");
    return NULL;
}