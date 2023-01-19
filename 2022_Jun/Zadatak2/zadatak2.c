#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int contains(const char* substring, const char* string) {
    int i = 0;
    while (string[i] != '\0') {
        int j = 1;
        if (string[i] == substring[0]) {
            while (string[i + j] == substring[j]) ++j;
            if (substring[j] == '\0') return 1;
        }
        i += j;
    }
    return 0;
}

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "pipe() has failed!\nThe process will now exit...\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork() has failed!\nThe process will now exit...\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) { //Parent Process
        close(pipefd[0]);
        char filePath[80];
        printf("File path: ");
        fgets(filePath, 80, stdin);
        int pathLength = strlen(filePath);
        filePath[pathLength - 1] = '\0';
        char keyword[19];
        printf("Keyword: ");
        scanf("%s", keyword);
        write(pipefd[1], filePath, pathLength);
        write(pipefd[1], keyword, strlen(keyword) + 1);
        close(pipefd[0]);
        printf("Parent> Exiting...\n");
    }
    else { //Child Process
        close(pipefd[1]);
        char buffer[99];
        int bytesReceived = read(pipefd[0], buffer, 99);
        printf("Bytes received: %d.\n", bytesReceived);
        char filePath[80];
        int i = 0;
        while (buffer[i] != '\0') {
            filePath[i] = buffer[i];
            ++i;
        }
        ++i;
        printf("Received: ->%s<-.\n", filePath);
        FILE* filep = fopen(filePath, "r");
        if (filep == NULL) {
            fprintf(stderr, "fopen() has failed!\nThe process will now exit...\n");
            exit(EXIT_FAILURE);
        }
        char keyword[19];
        int j = 0;
        while (buffer[i] != '\0') {
            keyword[j] = buffer[i];
            ++i; ++j;
        }
        printf("Received: ->%s<-.\n", keyword);
        char aLine[80];
        int count = 0;
        while (fgets(aLine, 80, filep) != NULL) {
            int doesExist = contains(keyword, aLine);
            doesExist ? printf("Line #%d contains the keyword (%s).\n", count++, keyword) : printf("Line #%d does not contain the keyword (%s).\n", count++, keyword);
        }
        close(pipefd[0]);
        fclose(filep);
        printf("Child> Exiting...\n");
    } 
    return 0;
}