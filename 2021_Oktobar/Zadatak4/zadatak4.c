#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

typedef struct MyFile {
    char filePath[PATH_MAX];
    long fileSize;
} MyFile;

int getMinIndex(MyFile inputArray[], int inputArrraySize);
void findLargestFiles(const char* directoryPath, MyFile largestFiles[]);

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char directoryPath[PATH_MAX]; strcpy(directoryPath, argv[1]);
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s].\n  Reason:", directoryPath);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    MyFile largestFiles[5];
    findLargestFiles(directoryPath, largestFiles);
    printf("The largest files inside the directory [%s] were: \n", directoryPath);
    for (int i = 0; i < 5; i++) printf(" %s: %ldB\n", largestFiles[i].filePath, largestFiles[i].fileSize);
    char* execvpArguments[7];
    execvpArguments[0] = (char*)malloc((strlen("rm") + 1) * sizeof(char));
    strcpy(execvpArguments[0], "rm");
    for (int i = 0; i < 5; ++i) {
        execvpArguments[i + 1] = (char*)malloc((strlen(largestFiles[i].filePath) + 1) * sizeof(char));
        strcpy(execvpArguments[i + 1], largestFiles[i].filePath);
    }
    execvpArguments[6] = NULL;
    int pid = fork();
    if (pid == -1) {
        perror("Error! Could not fork the process\n  Reason: ");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        if (execvp("rm", execvpArguments) == -1) {
            perror("Error! Could not execute the rm command\n  Reason: ");
            exit(EXIT_FAILURE);
        }
    }
    else wait(NULL);
    for (int i = 0; i < 5; ++i) free(execvpArguments[i]);
    closedir(directoryPointer);
    return 0;
}

int getMinIndex(MyFile inputArray[], int inputArrraySize) {
    int minIndex = 0;
    for (int i = 0; i < inputArrraySize; i++) minIndex = (inputArray[i].fileSize < inputArray[minIndex].fileSize) ? i : minIndex;
    return minIndex;
}

void findLargestFiles(const char* directoryPath, MyFile largestFiles[]) {
    DIR* directoryPointer = opendir(directoryPath);
    if (directoryPointer == NULL) {
        fprintf(stderr, "Error! Could not open directory [%s].\n  Reason:", directoryPath);
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
            fprintf(stderr, "Error! Could not get file stats [%s].\n  Reason:", newPath);
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(fileStats->st_mode)) findLargestFiles(newPath, largestFiles);
        if (S_ISREG(fileStats->st_mode)) {
            int minIndex = getMinIndex(largestFiles, 5);
            if (fileStats->st_size > largestFiles[minIndex].fileSize) {
                printf("Found a larger file [%s] ( %ldB ).\n", newPath, fileStats->st_size);
                MyFile newFile; strcpy(newFile.filePath, newPath); newFile.fileSize = fileStats->st_size;
                largestFiles[minIndex] = newFile;
            }
        }
    }
    closedir(directoryPointer);
    free(directoryEntry);
    free(fileStats);
}