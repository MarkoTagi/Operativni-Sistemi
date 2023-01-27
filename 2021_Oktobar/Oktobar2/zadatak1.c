#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#define MAX_LEN 255

void* countingThread(void* arg);
int stringToInteger(const char* string);

int main() {
    char userInput[MAX_LEN];
    do {
        printf("Enter something: ");
        fgets(userInput, MAX_LEN, stdin); fflush(stdin);
        int inputLength = strlen(userInput);
        userInput[--inputLength] = '\0';
        if (strcmp(userInput, "END") != 0) {
            int counter = stringToInteger(userInput);
            if (counter != 0) {
                pthread_t id_countingThread;
                pthread_create(&id_countingThread, NULL, countingThread, &counter);
                pthread_join(id_countingThread, NULL);
            }
        }
    } while (strcmp(userInput, "END") != 0);
    return 0;
}

int stringToInteger(const char* string) {
    int stringLength = strlen(string);
    int number = 0;
    for (int i = 0; i < stringLength; ++i) {
        int notDigit = !(string[i] >= '0' && string[i] <= '9');
        if (notDigit && (string[i] != '-' || i != 0)) {
            printf("Invalid input! [%s]\n", string);
            return 0;
        };
        int digit = (string[i] != '-') ? string[i] - '0' : -(string[++i] - '0');
        number += (number >= 0) ? digit * pow(10, stringLength - 1 - i) : - (digit * pow(10, stringLength - 1 - i));
    }
    return number;
}

void* countingThread(void* arg) {
    int counter = *(int*)arg;
    printf("Counter: ");
    for (int i = 0; i < abs(counter); ++i) {
        if (counter > 0) printf("%d...", counter - i);
        else printf("%d...", counter + i);
        fflush(stdout);
        sleep(1);
    }
    printf("0!\n");
    return NULL;
}