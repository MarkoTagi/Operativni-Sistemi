#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

struct ThreadArguments {
    char* sentence;
    int sentenceLength;
    int wordCount;
} typedef ThreadArguments;

char** separateWords(char* sentence, int wordCount) {
    //printf("separateWords> Word Count: %d.\n", wordCount);
    char** words = malloc(wordCount * sizeof(char*));
    int j = 0;
    for (int i = 0; i < wordCount; ++i) {
        int wordLength = 1;
        words[i] = malloc(wordLength * sizeof(char));
        while (sentence[j] != '\0' && sentence[j] != ' ') {
            //printf("separateWords> Value @%d: %c.\n", j, sentence[j]);
            words[i][wordLength - 1] = sentence[j];
            words[i] = realloc(words[i], ++wordLength);
            ++j;
        }
        words[i][wordLength - 1] = '\0';
        ++j;
        //printf("separateWords> words[%d] = %s.\n", i, words[i]);
    }
    return words;
}

int stringCompare(const char* string1, const char* string2) {
    char* strCopy1 = malloc(sizeof(string1) * sizeof(char));
    char* strCopy2 = malloc(sizeof(string2) * sizeof(char));
    strcpy(strCopy1, string1); strcpy(strCopy2, string2);
    //printf("String 1 copy: %s.\n", strCopy1);
    //printf("String 2 copy: %s.\n", strCopy2);
    int i = 0;
    do {
        bool isUppercase1 = string1[i] >= 'A' && string1[i] <= 'Z';
        if (isUppercase1) strCopy1[i] += 32;
        //printf("Check 1: %s.\n", strCopy1);
        bool isUppercase2 = string2[i] >= 'A' && string2[i] <= 'Z';
        if (isUppercase2) strCopy2[i] += 32;
        //printf("Check 2: %s.\n", strCopy2);
        if (strCopy1[i] > strCopy2[i]) {
            free(strCopy1); free(strCopy2);
            return 1;
        }
        else {
            free(strCopy1); free(strCopy2);
            return 2;
        }
        ++i;
    } while (string1[i] != '\0' && string2[i] != '\0');
    return 0;
}

void* printingThread(void* args) {
    printf("[PRINTING_THREAD] Formating the sentence...\n");
    char* userInput = (char*)args;
    if (userInput[0] >= 'a' && userInput[0] <= 'z') userInput[0] -= 32;
    int i = 1;
    while (userInput[i] != '\0') {
        if (userInput[i] == ' ') userInput[++i] += (userInput[i] >= 'A' && userInput[i] <= 'Z') ? 32 : 0;
        ++i;
    }
    printf("PRINTING_THREAD] Formatted sentence: %s.\n", userInput);
    printf("[PRINTING_THREAD] Exiting...\n");
}

void* sortingThread(void* args) {
    printf("[SORTING_THREAD] Sorting the sentence...\n");
    char* userInput = ((ThreadArguments*)args)->sentence;
    int inputLength = ((ThreadArguments*)args)->sentenceLength;
    int wordCount = ((ThreadArguments*)args)->wordCount;
    char** words = malloc(wordCount * sizeof(char*));
    int j = 0;
    for (int i = 0; i < wordCount; ++i) {
        int wordLength = 1;
        words[i] = malloc(wordLength * sizeof(char));
        while (userInput[j] != '\0' && userInput[j] != ' ') {
            //printf("separateWords> Value @%d: %c.\n", j, sentence[j]);
            words[i][wordLength - 1] = userInput[j];
            words[i] = realloc(words[i], ++wordLength);
            ++j;
        }
        words[i][wordLength - 1] = '\0';
        ++j;
        //printf("separateWords> words[%d] = %s.\n", i, words[i]);
    }
    char* sortedInput = malloc(inputLength * sizeof(char));
    for (int i = 0; i < wordCount - 1; ++i) {
        int maxIndex = i;
        for (int j = i + 1; j < wordCount; ++j) if (stringCompare(words[j], words[maxIndex]) == 1) maxIndex = j;
        //printf("Max word: %s.\n", words[maxIndex]);
        if (maxIndex != i) {
            char* temp = malloc(sizeof(words[i]) * sizeof(char));
            strcpy(temp, words[i]);
            words[i] = words[maxIndex];
            words[maxIndex] = temp;
        }
    }
    int k = 0;
    for (int i = 0; i < wordCount; ++i) {
        int j = 0;
        while (words[i][j] != '\0') {
            sortedInput[k] = words[i][j];
            ++j; ++k;
        }
        sortedInput[k] = ' ';
        ++k;
    }
    sortedInput[k] = '\0';
    printf("[SORTING_THREAD] Sorted sentence: %s.\n", sortedInput);
    //printf("[SORTING_THREAD] Sorted Input Length: %d.\n", k);
    strcpy(userInput, sortedInput);
    free(sortedInput);
    pthread_t printingThread_id;
    pthread_create(&printingThread_id, NULL, printingThread, userInput);
    pthread_join(printingThread_id, NULL);
    printf("[SORTING_THREAD] Exiting...\n");
}

int main() {
    char userInput[255];
    do {
        printf("[MAIN_THREAD] Enter a sentence: ");
        fgets(userInput, 255, stdin);
        if (userInput == NULL) printf("[MAIN_THREAD] You have to enter something!\n");
    } while (userInput == NULL);
    int inputLength = 0;
    int wordCount = 1;
    while (userInput[inputLength] != '\n') {
        ++inputLength;
        if (userInput[inputLength] == ' ') ++wordCount;
    }
    userInput[inputLength] = '\0';
    printf("[MAIN_THREAD] You've entered: %s.\n", userInput);
    //separateWords(userInput, wordCount);
    //printf("[MAIN_THREAD] Sentence length: %d.\n", inputLength);
    //printf("[MAIN_THREAD] Word count: %d.\n", wordCount);
    //for (int i = 0; i < inputLength; ++i) printf("Sentence[%d]: %c\n", i, userInput[i]);
    ThreadArguments ta;
    ta.sentence = userInput;
    ta.sentenceLength = inputLength;
    ta.wordCount = wordCount;
    pthread_t sortingThread_id;
    pthread_create(&sortingThread_id, NULL, sortingThread, &ta);
    pthread_join(sortingThread_id, NULL);
    printf("[MAIN_THREAD] Exiting..\n");
    return 0;
}
