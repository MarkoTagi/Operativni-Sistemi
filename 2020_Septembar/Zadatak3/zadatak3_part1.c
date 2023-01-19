#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#define MSGQ_ID 1337

int main() {
    int qid = msgget(MSGQ_ID, IPC_CREAT | 0666);
    if (qid == -1) {
        fprintf(stderr, "Error! [msgget]\n");
        exit(EXIT_FAILURE);
    }
    int pid1 = fork();
    if (pid1 == -1) {
        fprintf(stderr, "Error! [fork]\n");
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0) {
        if (execl("./zadatak3_part2", "zadatak3_part2", NULL) == -1) {
            fprintf(stderr, "Error! [execl(zadatak3_part2)]\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        int pid2 = fork();
        if (pid2 == -1) {
            fprintf(stderr, "Error! [fork]\n");
            exit(EXIT_FAILURE);
        }
        if (pid2 == 0) {
            if (execl("./zadatak3_part3", "zadatak3_part3", NULL) == -1) {
                fprintf(stderr, "Error! [execl(zadatak3_part3)]\n");
                exit(EXIT_FAILURE);
            }
        }
        else {
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        }
        msgctl(qid, IPC_RMID, NULL);
    }
    return 0;
}