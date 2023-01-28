#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int stringToInteger(const char* inputString);
void printFiles(const char* directoryPath, size_t minSize, int minLength);

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "bad usage: %s <absoluteDirectoryPath> <fileSize> <pathLength>\n", argv[0]);
        exit(EXIT_FAILURE); 
    }
    char directoryPath[PATH_MAX]; strcpy(directoryPath, argv[1]);
    if (directoryPath[0] != '/') {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "\tThe directory path must be absolute!\n");
        exit(EXIT_FAILURE);
    }
    int minSize = stringToInteger(argv[2]);
    if (errno == EINVAL) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "\tInvalid input [%s]! The minimum file size specified must be of type integer.\n", argv[2]);
        errno = 0;
        exit(EXIT_FAILURE);
    }
    else if (minSize < 0) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "\tInvalid input [%s]! The minimum file size must be a non negative integer.\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    int minLength = stringToInteger(argv[3]);
    if (errno == EINVAL) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "\tInvalid input [%s]! The minimum file path specified must be of type integer.\n", argv[3]);
        errno = 0;
        exit(EXIT_FAILURE);
    }
    else if (minLength < 0) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "\tInvalid input [%s]! The minimum file path must be a non negative integer.\n", argv[3]);
        exit(EXIT_FAILURE);
    }
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s]\n  Reason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    printFiles(directoryPath, minSize, minLength);
    closedir(directoryPointer);
    return 0;
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

void printFiles(const char* directoryPath, size_t minSize, int minLength) {
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s]\n  Reason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    errno = 0;
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        bool isSelfOrParent = (strcmp(directoryEntry->d_name, ".") == 0) || (strcmp(directoryEntry->d_name, "..") == 0);
        if (isSelfOrParent) continue;
        char newPath[PATH_MAX]; strcpy(newPath, directoryPath);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, fileStats) == -1) {
            fprintf(stderr, "Error! Could not stat [%s]\n  Reason: ", newPath); fflush(stderr);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(fileStats->st_mode)) printFiles(newPath, minSize, minLength);
        if (S_ISREG(fileStats->st_mode) && fileStats->st_size > minSize && strlen(newPath) > minLength) {
            printf("File name: %s\n", directoryEntry->d_name);
            printf("\tAbsolute path: %s ( %ld )\n", newPath, strlen(newPath));
            printf("\tSize: %ld\n", fileStats->st_size);
        }
    }
    if (errno != 0) {
        fprintf(stderr, "Error! Could not read directory [%s]\n Reason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    closedir(directoryPointer);
    free(fileStats);
    free(directoryEntry);
}