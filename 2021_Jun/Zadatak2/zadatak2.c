#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "zadatak2: bad usage\n");
        exit(EXIT_FAILURE);
    }
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "pipe() has failde!\nThe process will now exit...\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork() has failed!\nThe process will now exit...\n");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        //close(pipefd[1]);
        execlp(argv[1], argv[1], NULL);
    }
    else {
        close(pipefd[1]);
        dup2(pipefd[0], 0);
        //close(pipefd[0]);
        execlp("more", "more", "-n", argv[2], NULL);
    }
    return 0;
}