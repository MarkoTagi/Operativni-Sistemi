#include  <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

int stringToInteger(char* integer_s) {
    int stringLength = 0;
    while (integer_s[stringLength] != '\0') ++stringLength;
    int theNumber = 0;
    for (int i = 0; i < stringLength; ++i) {
        bool isCharacter = integer_s[i] < '0' || integer_s[i] > '9';
        if (isCharacter && integer_s[i] != '-') {
            printf("Invalid input, you've entered a string (%s).\n", integer_s);
            return 0;
        }
        int aDigit = integer_s[i] == '-' ? -(integer_s[++i] - '0') : integer_s[i] - '0';
        theNumber += aDigit * pow(10, stringLength - 1 - i);
    }
    return theNumber;
}

void* countingThread (void* arg) {
    int count = *((int*)arg);
    printf("======**COUNTING THREAD**======\n");
    if (count > 0) {
        for (int i = 0; i < count; ++i) {
            printf("%d...", count - i);
            fflush(stdout);
            sleep(1);
        }
    } else {
        for (int i = count; i < 0; ++i) {
            printf("%d...", i);
            fflush(stdout);
            sleep(1);
        }
    }
    printf("0.\n");
}

int main() {
    char userInput[255];
    pthread_t thread_id;
    do {
        printf("======**MAIN**======\nEnter an integer (or type \"END\" to exit): ");
        scanf   ("%s", userInput);
        if (strcmp(userInput, "END") != 0) {
            int inputNumber = stringToInteger(userInput);
            if (inputNumber) {
                pthread_create(&thread_id, NULL, countingThread, &inputNumber);
                pthread_join(thread_id, NULL);
            }
        }
    } while (strcmp(userInput, "END") != 0);
    printf("======**MAIN**======\nExiting...\n");
    return 0;
}
