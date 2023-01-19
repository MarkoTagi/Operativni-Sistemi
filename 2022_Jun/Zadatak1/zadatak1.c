#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct SortThreadArgs {
    int* array;
    int arraySize;
}typedef SortThreadArgs;

int stringToInteger(const char* string) {
    int stringLength = 0;
    while (string[stringLength] != '\n') ++stringLength;
    int theNumber = 0;
    for (int i = 0; i < stringLength; ++i) {
        int aDigit = (string[i] != '-') ? string[i] - '0' : -(string[++i] - '0');
        theNumber += aDigit * pow(10, stringLength - 1 - i);
    }
    return theNumber;
}

void printArray(int inputArray[], int arraySize) {
    printf("[ ");
    for (int i = 0; i < arraySize - 1; ++i) printf("%d, ", inputArray[i]);
    printf("%d ]\n", inputArray[arraySize - 1]);
}

void* sortingThread(void* args) {
    printf("[SORTING_THREAD] Sorting...\n");
    int* theArray = ((SortThreadArgs*)args)->array;
    int arraySize = ((SortThreadArgs*)args)->arraySize;
    for (int i = 0; i < arraySize - 1; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < arraySize; ++j) minIndex = (theArray[j] < theArray[minIndex]) ? j : minIndex;
        if (minIndex != i) {
            int temp = theArray[i];
            theArray[i] = theArray[minIndex];
            theArray[minIndex] = temp;
        }
    }
    printf("[SORTING_THREAD] Done sorting.\n");
}

int main() {
    FILE* fp = fopen("./ulaz.txt", "r");
    if (fp == NULL) {
        printf("An error occured while trying to open the file. Exiting...\n");
        return -1;
    }
    int arraySize = 1;
    int* array = malloc(arraySize * sizeof(int));
    char currentLine[10];
    while (fgets(currentLine, 10, fp) != NULL) {
        int currentNumber = stringToInteger(currentLine);
        array[arraySize - 1] = currentNumber;
        ++arraySize;
        array = realloc(array, arraySize);
    }
    array = realloc(array, --arraySize);
    fclose(fp);
    pthread_t sortingThread_id;
    printf("[MAIN] Unsorted array: ");
    printArray(array, arraySize);
    SortThreadArgs sta;
    sta.array = array;
    sta.arraySize = arraySize;
    pthread_create(&sortingThread_id, NULL, sortingThread, &sta);
    pthread_join(sortingThread_id, NULL);
    printf("[MAIN] Sorted array: ");
    printArray(array, arraySize);
    printf("[MAIN] Exiting...\n");
    free(array);
    return 0;
}
