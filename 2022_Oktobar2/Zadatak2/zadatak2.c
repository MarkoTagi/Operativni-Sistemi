#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void printArray(char* inputArray[], int arrayLength) {
    printf("[ ");
    for (int i = 0; i < arrayLength - 1; ++i) printf("%s, ", inputArray[i]);
    printf("%s ]\n", inputArray[arrayLength - 1]);
}

int main(int argc, char** argv) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "pipe() has failed!\n The process will now exit.\n");
        exit(EXIT_FAILURE);
    }
    int standardOutput = dup(1);
    if (dup2(pipefd[1], 1) < 0) {
        fprintf(stderr, "dup2() has failed!\n The process will now exit.\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork() has failed!\n The process will now exit.\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) { // Parent process code
        char** arguments = malloc(argc * sizeof(char*));
        int totalStringLength = 0;
        for (int i = 0; i < argc; ++i) {
            int stringLength = strlen(argv[argc - 1 - i]) + 1;
            totalStringLength += stringLength;
            arguments[i] = malloc(stringLength * sizeof(char));
            strcpy(arguments[i], argv[argc - 1 - i]);
        }
        totalStringLength += argc -1;
        char* argumentsString = malloc(totalStringLength * sizeof(char));
        for (int i = 0; i < argc - 1; ++i) {
            strcat(argumentsString, arguments[i]);
            strcat(argumentsString, " ");
        }
        strcat(argumentsString, arguments[argc - 1]);
        close(pipefd[0]);
        for (int i = 0; i < argc; ++i) {
            int execlpStatus = execl("/bin/echo", "/bin/echo", argumentsString, NULL);
            if (execlpStatus < 0) fprintf(stderr, "execlp() has failed!\n");
        }
        wait(NULL);
        for (int i = 0; i < argc + 1; ++i) free(arguments[i]);
        free(arguments);
        free(argumentsString);
        close(pipefd[1]);
        printf("Parent> Exiting...\n");
    }
    else { // Child process code
        close(pipefd[1]);
        char buffer[80];
        read(pipefd[0], buffer, 80);
        dup2(standardOutput, 1);
        printf("Received: %s", buffer);
        close(pipefd[0]);
        printf("Child> Exiting...\n");
    }
    return 0;
}

