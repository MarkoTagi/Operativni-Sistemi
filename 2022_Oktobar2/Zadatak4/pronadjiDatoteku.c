#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

bool findFile(const char* directorypPath, const char* fileName);

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "options: %s <directory path> <file name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char fileName[PATH_MAX]; strcpy(fileName, argv[2]);
    char directoryPath[PATH_MAX]; strcpy(directoryPath, argv[1]);
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Failed to open directory [%s]\n\tReason: ", directoryPath);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    bool wasFound  = findFile(directoryPath, fileName);
    if (!wasFound) printf("The file [%s] does not exist inside of the directory [%s]\n", fileName, directoryPath);
    closedir(directoryPointer);
}

bool findFile(const char* directoryPath, const char* fileName) {
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Failed to open directory [%s]\n\tReason: ", directoryPath);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    bool wasFound = false;
    struct dirent* directoryEntry = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* fileStats = (struct stat*)malloc(sizeof(struct stat));
    while ((directoryEntry = readdir(directoryPointer)) != NULL) {
        bool isSelfOrParent = (strcmp(directoryEntry->d_name, ".") == 0) || (strcmp(directoryEntry->d_name, "..") == 0);
        if (isSelfOrParent) continue;
        char newPath[PATH_MAX]; strcpy(newPath, directoryPath);
        strcat(newPath, "/"); strcat(newPath, directoryEntry->d_name);
        if (stat(newPath, fileStats) == -1) {
            fprintf(stderr, "Failed to get file statistics [%s]\n\tReason: ", newPath);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(fileStats->st_mode)) return findFile(newPath, fileName); // Nalazi prvu pojavu i izlazi, ukloni "return" ako zelis da pronadje sve fajlove
        if (S_ISREG(fileStats->st_mode) && strcmp(fileName, directoryEntry->d_name) == 0) {
            printf("Found file [%s]!\n", fileName);
            printf("  Location: %s\n", newPath);
            printf("  Size: %ld\n", fileStats->st_size);
            return true;
        }
    }
    closedir(directoryPointer);
    free(directoryEntry);
    free(fileStats);
    return wasFound;
}