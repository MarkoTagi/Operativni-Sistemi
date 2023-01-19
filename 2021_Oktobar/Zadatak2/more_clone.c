#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#define PARAMS 0666

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "more_clone: bad usage\n");
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
    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        execlp(argv[1], argv[1], NULL);
    }
    else {
        close(pipefd[1]);
        dup2(pipefd[0], 0);
        execlp("more", "more", "-n", argv[2], NULL);
    }
    return 0;
}
