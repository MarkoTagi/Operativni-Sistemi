#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_DEPTH 3

void removeFiles(const char* directoryPath, size_t greaterThanKBs, int currentDepth);

int main() {
    char directoryPath[PATH_MAX];
    printf("Enter directory path: ");
    fgets(directoryPath, sizeof(directoryPath), stdin);
    int directoryPathLength = strlen(directoryPath);
    directoryPath[--directoryPathLength] = '\0';
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s]\n  Reason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    removeFiles(directoryPath, 10, 0);
    closedir(directoryPointer);
    return 0;
}

void removeFiles(const char* directoryPath, size_t greaterThanKBs, int currentDepth) {
    if (currentDepth > MAX_DEPTH) {
        printf("Maximum depth ( %d ) reached. Returning...\n", MAX_DEPTH);
        return;
    }
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s]\n  Reason: ", directoryPath); fflush(stderr);
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
            fprintf(stderr, "Error! Could not get file statistics [%s]\n  Reason: ", newPath); fflush(stderr);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(fileStats->st_mode)) removeFiles(newPath, greaterThanKBs, currentDepth + 1);
        if (S_ISREG(fileStats->st_mode) && (fileStats->st_size > (greaterThanKBs * 1024))) {
            int pid = fork();
            if (pid == -1) {
                perror("Error! Could not fork the process\n  Reason:");
                exit(EXIT_FAILURE);
            }
            if (pid == 0) {
                printf("Removing file [%s] ( %ldB )...\n", newPath, fileStats->st_size);
                if (execlp("rm", "rm", newPath, NULL) == -1) {
                    fprintf(stderr, "Error! Could not remove file [%s]\n  Reason: ", newPath); fflush(stderr);
                    perror(NULL);
                    exit(EXIT_FAILURE);
                }
            }
            else wait(NULL);
        }
    }
    closedir(directoryPointer);
    free(fileStats);
    free(directoryEntry);
}