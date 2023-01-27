#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX_LEN 1024

int main(int argc, char** argv) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "Error! [ pipe() has failed ]\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error! [ fork() has failed ]\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        close(pipefd[1]);
        int argsLength = 0;
        char** argsReversed = malloc(argc * sizeof(char*));
        for (int i = 0; i < argc; ++i) {
            int argLength = strlen(argv[argc - 1 - i]);
            argsLength += argLength + 1;
            argsReversed[i] = malloc((argLength + 1) * sizeof(char));
            strcpy(argsReversed[i], argv[argc - 1 - i]);
            printf("argsReversed[%d] = %s\n", i, argsReversed[i]);
        }
        char* argsString = malloc(argsLength * sizeof(char));
        for (int i = 0; i < argc - 1; ++i) {
            strcat(argsString, argsReversed[i]);
            strcat(argsString, " ");
            free(argsReversed[i]); argsReversed[i] = NULL;
        }
        strcat(argsString, argsReversed[argc - 1]);
        free(argsReversed[argc - 1]); argsReversed[argc - 1] = NULL;
        free(argsReversed); argsReversed = NULL;
        if (execl("/bin/echo", "/bin/echo", argsString, NULL) == -1) {
            fprintf(stderr, "Error! [ echo has failed ]\n");
            free(argsString); argsString = NULL;
            exit(EXIT_FAILURE);
        }
    }
    else {
        close(pipefd[1]);
        char buffer[MAX_LEN];
        int bytesReceived;
        if ((bytesReceived = read(pipefd[0], buffer, MAX_LEN)) == -1) {
            fprintf(stderr, "Error! [ read() has failed ]\n");
            exit(EXIT_FAILURE);
        }
        else {
            buffer[bytesReceived - 1] = '\0';
            printf("Reversed arguments: %s.\n", buffer);
        }
        close(pipefd[0]);
    }
    return 0;
}