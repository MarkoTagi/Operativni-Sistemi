#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#define MAX_LEN 1024

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "zadatak2: bad usage\n");
        exit(EXIT_FAILURE);
    }
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
        dup2(pipefd[1], 1);
        close(pipefd[1]);
        execlp(argv[1], argv[1], NULL);
    }
    else {
        close(pipefd[1]);
        FILE* filep = fopen(argv[2], "w");
        if (filep == NULL) {
            fprintf(stderr, "fopen(%s) has failed!\n", argv[2]);
            exit(EXIT_FAILURE);
        }
        char buffer[MAX_LEN];
        int bytesReceived = read(pipefd[0], buffer, MAX_LEN);
        close(pipefd[0]);
        for (int i = 0; i < bytesReceived; ++i) {
            char c = buffer[i];
            if (c == 'b') fputc('B', filep);
            else fputc(c, filep);
        }
        fclose(filep);
    }
    return 0;
}