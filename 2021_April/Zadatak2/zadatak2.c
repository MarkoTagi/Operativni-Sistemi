#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>

//   0         1           2           3
//zadatak2 <src_file> <byte_count> <dst_file>

int stringToInteger(const char* string) {
    int stringLength = strlen(string);
    int theNumber = 0;
    for (int i = 0; i < stringLength; ++i) {
        int notDigit = string[i] < '0' || string[i] > '9';
        if (notDigit) return -1;
        int aDigit = string[i] - '0';
        theNumber += aDigit * pow(10, stringLength - 1 - i);  
    } 
    return theNumber;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "zadatak2: bad usage\n");
        exit(EXIT_FAILURE);
    }
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
    if (pid > 0) {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        close(pipefd[1]);
        FILE* filep = fopen(argv[1], "r");
        if (filep == NULL) {
            fprintf(stderr, "Could not open file (%s). The process will now exit...\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        int argv2 = stringToInteger(argv[2]);
        if (argv2 == -1) {
            fprintf(stderr, "zadatak2: bad usage\n");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < argv2; ++i) {
            char c = getc(filep);
            fprintf(stderr, "char = %c\n", c);
            if (c == EOF) break;
            putc(c, stdout);
        }
        fflush(NULL);
        fclose(filep);
        fprintf(stderr, "Parent exiting...\n");
    }
    else {
        close(pipefd[1]);
        FILE* filep = fopen(argv[3], "w");
        if (filep == NULL) {
            fprintf(stderr, "Could not open file (%s). The process will now exit...\n", argv[3]);
            exit(EXIT_FAILURE);
        }
        int bytes_sent = stringToInteger(argv[2]);
        char* buffer = malloc((bytes_sent + 1) * sizeof(char));
        read(pipefd[0], buffer, bytes_sent);
        buffer[bytes_sent] = '\0';
        for (int i = 0; i < bytes_sent; ++i) putc(buffer[i], filep);
        close(pipefd[0]);
        fclose(filep);
        free(buffer);
    }
    return 0;
}