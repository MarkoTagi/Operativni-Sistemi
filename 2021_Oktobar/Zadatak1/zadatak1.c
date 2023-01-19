#include <stdio.h>
#include <pthread.h>
#include <string.h>

struct PrintArgs {
    char* string;
    int stringLength;
};


void* printingThread(void* arg) {
    char* sortedUserInput = ((struct PrintArgs*)arg)->string;
    int stringLength = ((struct PrintArgs*)arg)->stringLength;
    if (sortedUserInput[0] > 'Z') sortedUserInput[0] = sortedUserInput[0] - ' ';
    if (sortedUserInput[stringLength - 1] != '.' && sortedUserInput[stringLength - 1] >= 'A') {
        sortedUserInput[stringLength] = '.';
        sortedUserInput[++stringLength] = '\0';
    }
    printf("Formated sorted string: (%s)\n", sortedUserInput);
}

void* sortingThread(void* arg) {
    char* userInput = (char*)arg;
    int i = 0; int stringLength = 0;
    while (userInput[i + 1] != '\n') {
        if (userInput[i] >= 'A') {
            int minIndex = i;
            int j = i + 1;
            while (userInput[j] != '\n') {
                if (userInput[j] >= 'A') minIndex = (userInput[j] < userInput[minIndex]) ? j : minIndex;
                ++j;
            }
            if (minIndex != i) {
                int temp = userInput[i];
                userInput[i] = userInput[minIndex];
                userInput[minIndex] = temp;
            }
        }
    ++i;
    }
    stringLength += i + 1;
    userInput[stringLength] = '\0';
    userInput[stringLength + 1] = '\0';
    printf("String length: %d.\n", stringLength);
    printf("Sorted string: (%s)\n", userInput);
    pthread_t printingThread_id;
    struct PrintArgs pa;
    pa.string = userInput;
    pa.stringLength = stringLength;
    pthread_create(&printingThread_id, NULL, printingThread, &pa);
    pthread_join(printingThread_id, NULL);
}


int main() {
    char userInput[255];
    printf("Enter a sentence: ");
    fgets(userInput, 255, stdin);
    printf("You've entered: %s\n", userInput);
    pthread_t sortingThread_id;
    pthread_create(&sortingThread_id, NULL, sortingThread, userInput);
    pthread_join(sortingThread_id, NULL);
    printf("Exiting...\n");
    return 0;
}
