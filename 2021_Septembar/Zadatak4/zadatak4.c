#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int stringToInteger(const char* inputString);
void readFiles(const char* directoryPath, int maxDepth, int currentDepth);

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "\toptions: %s <absoluteDirectoryPath> <maxDepth>\n", argv[0]);
        exit(EXIT_FAILURE); 
    }
    char directoryPath[PATH_MAX]; strcpy(directoryPath, argv[1]);
    if (directoryPath[0] != '/') {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "\tThe directory path must be absolute!\n");
        exit(EXIT_FAILURE);
    }
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s].\n  Reason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    int maxDepth = stringToInteger(argv[2]);
    if (errno == EINVAL) {
        fprintf(stderr, "Error! Invalid max depth [%s].\n  Reason: Not a number.\n", argv[2]);
        errno = 0;
        exit(EXIT_FAILURE);
    }
    else if (maxDepth < 0) {
        fprintf(stderr, "Error! Invalid max depth [%s].\n  Reason: Must be a non negative value.\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    readFiles(directoryPath, maxDepth, 0);
    closedir(directoryPointer);
}

int stringToInteger(const char* inputString) {
    int inputLength = strlen(inputString);
    int number = 0;
    for (int i = 0; i < inputLength; ++i) {
        bool notDigit = inputString[i] < '0' || inputString[i] > '9';
        if (notDigit && (inputString[i] != '-' || i != 0)) {
            errno = EINVAL;
            return -1;
        }
        int digit = (inputString[i] != '-') ? inputString[i] - '0' : -(inputString[++i] - '0');
        number += (number >= 0) ? digit * pow(10, inputLength - 1 - i) : -(digit * pow(10, inputLength - 1 - i)); 
    }
    return number;
}

void readFiles(const char* directoryPath, int maxDepth, int currentDepth) {
    if (maxDepth != 0 && currentDepth > maxDepth) {
        printf("Maximum depth ( %d ) reached. Returning...\n", maxDepth);
        return;
    }
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s].\n  Reason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        bool isSelfOrParent = (strcmp(directoryEntry->d_name, ".") == 0) || (strcmp(directoryEntry->d_name, "..") == 0);
        if (isSelfOrParent) continue;
        char newPath[PATH_MAX]; strcpy(newPath, directoryPath);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, fileStats) == -1) {
            fprintf(stderr, "Error! Could not get file statistics [%s].\n  Reason:", newPath); fflush(stderr);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(fileStats->st_mode)) readFiles(newPath, maxDepth, currentDepth + 1);
        if (S_ISREG(fileStats->st_mode) && !(S_IXUSR & fileStats->st_mode)) {
            FILE* filePointer = fopen(newPath, "r");
            if (filePointer == NULL) {
                fprintf(stderr, "Error! Could not open file [%s].\n  Reason:", newPath); fflush(stderr);
                perror(NULL);
                exit(EXIT_FAILURE);
            }
            printf("\nReading file [%s]...\n\n", newPath);
            char lineBuffer[1024];
            int lineCount = 0;
            while(((fgets(lineBuffer, 1024, filePointer)) != NULL) && lineCount < 10) printf("%d %s", (lineCount++) + 1, lineBuffer);
            printf("\nFinished reading file [%s]!\n", newPath);
            fclose(filePointer);
        }
    }
    closedir(directoryPointer);
    free(fileStats);
    free(directoryEntry);
}