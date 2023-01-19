#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct String {
    int maxLength;
    int stringLength;
    char* theString;
} typedef String;

void initializeString(String* string) {
    string->maxLength = 19;
    string->stringLength = 0;
    string->theString = malloc(string->maxLength * sizeof(char));
}

String getString(String string) {
    char userInput;
    do {
        userInput = getc(stdin);
        ++string.stringLength;
        if (userInput != '\n') {
            if(string.stringLength > string.maxLength) {
                string.maxLength *= 2;
                string.theString = realloc(string.theString, string.maxLength);
            }
            string.theString[string.stringLength-1] = userInput;
        }
    } while (userInput != '\n');
}

void printString(String string) {
    printf("%s\n", string.theString);
}

void stringInfo(String string) {
    printf("string.theString: %s\nstring.stringLength: %d\nstring.maxLength: %d\n", string.theString, string.stringLength, string.maxLength);
}

int main() {
    String userInput;
    initializeString(&userInput);
    printf("Main> Enter something: ");
    getString(userInput);
    stringInfo(userInput);
    return 0;
}
