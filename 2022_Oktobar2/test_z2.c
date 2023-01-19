#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char* cmd_arguments_s;

int main(int argc, char** argv) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "pipe() has failed!\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork() has failed!\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        close(pipefd[0]);
        char* cmd_arguments[argc];
        int argumentsLength = 0;
        for (int i = 0; i < argc; ++i) {
            int stringLength = strlen(argv[argc - 1 -i]);
            cmd_arguments[i] = malloc(++stringLength * sizeof(char));
            strcpy(cmd_arguments[i], argv[argc - 1 - i]);
            argumentsLength += stringLength + 1;
        }
        cmd_arguments_s = malloc(argumentsLength * sizeof(char));
        for (int i = 0; i < argc - 1; ++i) {
            strcat(cmd_arguments_s, cmd_arguments[i]); strcat(cmd_arguments_s, " ");
        }
        strcat(cmd_arguments_s, cmd_arguments[argc - 1]);
        for (int i = 0; i < argc; ++i) free(cmd_arguments[i]);
        dup2(pipefd[1], 1);
        execlp("echo", "echo", cmd_arguments_s, NULL);
    }
    else {
        close(pipefd[1]);
        char buffer[1024];
        read(pipefd[0], buffer, 1024);
        printf("[In child process] %s", buffer);
        close(pipefd[0]);
        free(cmd_arguments_s);
    }
    return 0;
}