#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

void findFile(const char* fileName, const char* filePath, const char* directoryPath);

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char directoryPath1[PATH_MAX]; strcpy(directoryPath1, argv[1]);
    DIR* directoryPointer1 = opendir(directoryPath1);
    if (directoryPointer1 == NULL) {
        fprintf(stderr, "Cannot open directory [%s]\n  Reason:", directoryPath1);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    char directoryPath2[PATH_MAX]; strcpy(directoryPath2, argv[2]);
    DIR* directoryPointer2 = opendir(directoryPath2);
    if (directoryPointer2 == NULL) {
        fprintf(stderr, "Cannot open directory [%s]\n  Reason:", directoryPath2);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    while ((directoryEntry = readdir(directoryPointer1)) != NULL) {
        bool isSelfOrParent = (strcmp(directoryEntry->d_name, ".") == 0) && (strcmp(directoryEntry->d_name, "..") == 0);
        if (isSelfOrParent) continue;
        char newPath[PATH_MAX]; strcpy(newPath, directoryPath1);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, fileStats) == -1) {
            fprintf(stderr, "Cannot get file statistics [%s]\n  Reason:", newPath);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        if (S_ISREG(fileStats->st_mode)) findFile(directoryEntry->d_name, newPath, directoryPath2);
    }
    closedir(directoryPointer1);
    closedir(directoryPointer2);
    free(directoryEntry);
    free(fileStats);
    return 0;
}

void findFile(const char* fileName, const char* filePath, const char* directoryPath) {
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Cannot open directory [%s]\n  Reason:", directoryPath);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* newFileStats = (struct stat*)malloc(sizeof(struct stat));
    struct stat* oldFileStats = (struct stat*)malloc(sizeof(struct stat));
    if (stat(filePath, oldFileStats) == -1) {
        fprintf(stderr, "Cannot get file statistics [%s]\n  Reason:", filePath);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    while ((directoryEntry = readdir(directoryPointer))!= NULL) {
        bool isSelfOrParent = (strcmp(directoryEntry->d_name, ".") == 0) || (strcmp(directoryEntry->d_name, "..") == 0);
        if (isSelfOrParent) continue;
        char newPath[PATH_MAX]; strcpy(newPath, directoryPath);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, newFileStats) == -1) {
            fprintf(stderr, "Cannot get file statistics [%s]\n  Reason:", newPath);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(newFileStats->st_mode)) findFile(fileName, filePath, newPath);
        if (S_ISREG(newFileStats->st_mode) && (strcmp(directoryEntry->d_name, fileName) == 0) && (newFileStats->st_size == oldFileStats->st_size)) printf("Found a copy of the file [%s] inside [%s]\n", fileName, newPath);
    }
    closedir(directoryPointer);
    free(directoryEntry);
    free(oldFileStats);
    free(newFileStats);
}