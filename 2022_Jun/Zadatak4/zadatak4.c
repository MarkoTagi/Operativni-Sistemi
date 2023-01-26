#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#define MAX_DEPTH 3

void findAndRemoveLogs(const char* directoryPath, size_t greaterThanKBs, int currentDepth);
bool containsString(const char* text, const char* pattern);

int main() {
    char directoryPath[PATH_MAX];
    printf("Enter directory path: ");
    fgets(directoryPath, PATH_MAX, stdin);
    int directoryPathLength = strlen(directoryPath);
    directoryPath[--directoryPathLength] = '\0';
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Failed to open directory [%s]!\n\tReason: ");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    findAndRemoveLogs(directoryPath, 15, 0);
    closedir(directoryPointer);
}

void findAndRemoveLogs(const char* directoryPath, size_t greaterThanKBs, int currentDepth) {
    if (currentDepth > MAX_DEPTH) {
        printf("Maximum depth reached! Returning...\n");
        return;
    }
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Failed to open directory [%s]!\n\tReason: ", directoryPath);
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
            fprintf(stderr, "Failed to get file statistics [%s]!\n\tReason: ", newPath);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(fileStats->st_mode)) findAndRemoveLogs(newPath, greaterThanKBs, currentDepth + 1);
        if (S_ISREG(fileStats->st_mode) && containsString(directoryEntry->d_name, "log") && fileStats->st_size > (greaterThanKBs * 1024)) {
            int pid = fork();
            if (pid == -1) {
                perror("Failed to fork process! Reason: ");
                exit(EXIT_FAILURE);
            }
            if (pid == 0) {
                if (execl("rm", "rm", newPath, NULL) == -1) {
                    fprintf(stderr, "Failed to remove file [%s]!\n\tReason:", newPath);
                    perror(NULL);
                    exit(EXIT_FAILURE);
                }
            }
            else wait(NULL);
        }
    }
    closedir(directoryPointer);
    free(directoryEntry);
    free(fileStats);
}

bool containsString(const char* text, const char* pattern) {
    int textLength = strlen(text); int patternLength = strlen(pattern);
    int textPosition = 0;
    while (text[textPosition] != '\0') {
        if ((textLength - textPosition) < patternLength) return false;
        int patternPosition = 0;
        if (text[textPosition] == pattern[patternPosition]) {
            while (text[++textPosition] == pattern[++patternPosition]) ;
            if (text[textPosition] == '\0' || pattern[patternPosition] == '\0') return true;
        }
        textPosition += patternPosition + 1;
    }
    return false;
}
