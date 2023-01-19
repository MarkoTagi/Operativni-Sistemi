#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#define MAX_LEN 8

int stringToInteger(const char* string) {
    int stringLength = strlen(string);
    int theNumber = 0;
    for (int i = 0; i < stringLength; ++i) {
        int notDigit = !(string[i] >= '0' && string[i] <= '9');
        if (notDigit && (string[i] != '-' || i != 0)) return -1;
        int aDigit = (string[i] == '-') ? -(string[++i] - '0') : string[i] - '0'; 
        theNumber += (theNumber >= 0) ? aDigit * pow(10, stringLength - 1 - i) : -(aDigit * pow(10, stringLength - 1 - i));
        //printf("The Digit = %d\n", aDigit); 
    }
    //printf("Converted \"%s\" into %d!\n", string, theNumber);
    return theNumber;
}

void* countingThread (void* arg) {
    int counter = *((int*)arg);
    printf("Counter: ");
    for (int i = 0; i < counter; ++i) {
        printf("%d...", i);
        fflush(stdout);
        sleep(3);
    }
    printf("%d!\n", counter);
}

int main() {
    char userInput[MAX_LEN];
    do {
        printf("Enter a positive integer (or type \"END\" to exit): ");
        fgets(userInput, MAX_LEN, stdin);
        int inputLength = strlen(userInput);
        userInput[inputLength - 1] = '\0';
        //printf("User has entered: (%s)\n", userInput);
        if (strcmp(userInput, "END") != 0) {
            int counter = stringToInteger(userInput);
            if (counter > 0) {
                pthread_t countingThread_id;
                pthread_create(&countingThread_id, NULL, countingThread, &counter);
                pthread_join(countingThread_id, NULL);
            }
            else printf("Enter a postitive integer!\n");
        }
    } while (strcmp(userInput, "END") != 0);
    return 0;
}