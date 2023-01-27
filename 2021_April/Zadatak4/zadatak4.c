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

int liveElements = 0;

typedef struct MyFile {
    char fileName[PATH_MAX];
    char filePath[PATH_MAX];
    long fileSize;
} MyFile;

void getFiles(const char* directoryPath, size_t greaterThanKBs, MyFile files[]);
void sortFiles(MyFile files[], int fileCount);

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        fprintf(stderr, "\toptions: %s <absoluteDirectoryPath>\n", argv[0]);
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
        fprintf(stderr, "Error! Could not open the directory [%s].\n  Reason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    MyFile sortedFiles[100];
    getFiles(directoryPath, 100, sortedFiles);
    sortFiles(sortedFiles, liveElements);
    for (int i = 0; i < liveElements; ++i) {
        printf("File name: %s\n", sortedFiles[i].fileName);
        printf("\tFile path: %s\n", sortedFiles[i].filePath);
        printf("\tFile size: %ldB\n", sortedFiles[i].fileSize);
        printf("\n");
    }
    closedir(directoryPointer);
}

void getFiles(const char* directoryPath, size_t greaterThanKBs, MyFile files[]) {
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open the directory [%s].\n  Reason: ", directoryPath); fflush(stderr);
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
            fprintf(stderr, "Error! Could not stat the file [%s].\n  Reason: ", newPath); fflush(stderr);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(fileStats->st_mode)) getFiles(newPath, greaterThanKBs, files);
        if (S_ISREG(fileStats->st_mode) && fileStats->st_size > (greaterThanKBs * 1024)) {
            MyFile newFile; strcpy(newFile.fileName, directoryEntry->d_name); strcpy(newFile.filePath, newPath); newFile.fileSize = fileStats->st_size;
            files[liveElements++] = newFile;
        }
    }
    if (errno != 0) {
        fprintf(stderr, "Error! Could not read the directory [%s].\n  Reason: ", directoryPath); fflush(stderr);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    free(fileStats);
    free(directoryEntry);
    closedir(directoryPointer);
}

void sortFiles(MyFile files[], int fileCount) {
    for (int i = 0; i < (fileCount - 1); ++i) {
        int minFileIndex = i;
        for (int j = i + 1; j < fileCount; ++j)
            if (files[j].fileSize < files[minFileIndex].fileSize) minFileIndex = j;
        if (minFileIndex != i) {
            MyFile temp = files[i];
            files[i] = files[minFileIndex];
            files[minFileIndex] = temp;
        }
    }
}