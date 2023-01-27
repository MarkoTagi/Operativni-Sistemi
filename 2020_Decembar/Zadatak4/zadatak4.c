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

void printFiles(const char* directoryPath, long minSize_KB, long maxSize_KB, int currentDepth);

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "\toptions: %s <absoluteDirectoryPath>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char directoryPath[PATH_MAX]; strcpy(directoryPath, argv[1]);
    if (directoryPath[0] != '/') {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "\tThe specified directory path must be absolute!\n");
        exit(EXIT_FAILURE);
    }
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s]\n\tReason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    printFiles(directoryPath, 100, 750, 0);
    closedir(directoryPointer);
    return 0;
}

void printFiles(const char* directoryPath, long minSize_KB, long maxSize_KB, int currentDepth) {
    if (currentDepth > 3) {
        printf("Maximum depth (  %d ) exceeded. Returning...\n", 3);
        return;
    }
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s]\n\tReason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    errno = 0;
    while((directoryEntry = readdir(directoryPointer)) != NULL) {
        bool isSelfOrParent = (strcmp(directoryEntry->d_name, ".") == 0) || (strcmp(directoryEntry->d_name, "..") == 0);
        if (isSelfOrParent) continue;
        char newPath[PATH_MAX]; strcpy(newPath, directoryPath);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, fileStats) == -1) {
            fprintf(stderr, "Error! Could not get file statistics [%s]\n\tReason: ", newPath); fflush(stderr);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(fileStats->st_mode)) printFiles(newPath, minSize_KB, maxSize_KB, currentDepth + 1);
        if (S_ISREG(fileStats->st_mode) && fileStats->st_size > (minSize_KB * 1024) && fileStats->st_size < (maxSize_KB * 1024)) {
            printf("File name: %s\n", directoryEntry->d_name);
            printf("\tAbsolute path of file: %s\n", newPath);
            printf("\tFile size: %fKB\n", ((double)fileStats->st_size / 1024));
        }
    }
    if (errno != 0) {
        fprintf(stderr, "Error! Could not read directory [%s]\n\tReason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    closedir(directoryPointer);
    free(fileStats);
    free(directoryEntry);
}