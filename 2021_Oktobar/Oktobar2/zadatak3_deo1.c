#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#define MSGQ_ID 1337
#define MAX_LEN 255

typedef struct Message {
    long messageType;
    char messageContent[MAX_LEN];
} Message;

int main() {
    int qid = msgget(MSGQ_ID, IPC_CREAT | 0666);
    if (qid == -1) {
        fprintf(stderr, "Error! [ msgget() has failed ]\n");
        exit(EXIT_FAILURE);
    }
    int pid1 = fork();
    if (pid1 == -1) {
        fprintf(stderr, "Error! [ fork() has failed ]\n");
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0) { //Drugi proces
        if (execl("./zadatak3_deo2", "zadatak3_deo2", NULL) == -1) {
            fprintf(stderr, "Error! [ zadatak3_deo2 failed to start ]\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        int pid2 = fork();
        if (pid2 == -1) {
            fprintf(stderr, "Error! [ fork() has failed ]\n");
            exit(EXIT_FAILURE);
        }
        if (pid2 == 0) { //Treci proces
            if (execl("./zadatak3_deo3", "zadatak3_deo3", NULL) == -1) {
                fprintf(stderr, "Error! [ zadatak3_deo3 failed to start ]\n");
                exit(EXIT_FAILURE);
            }
        }
        else { //Prvi proces
            int processNumber;
            Message buffer;
            do {
                printf("Which process should receive the message? ");
                scanf("%d", &processNumber); fgetc(stdin);
                if (processNumber == 1 || processNumber == 2) {
                    printf("What would you like to send? ");
                    fgets(buffer.messageContent, MAX_LEN, stdin);
                    int messageLength = strlen(buffer.messageContent);
                    buffer.messageContent[--messageLength] = '\0';
                    if (processNumber == 1) {
                        buffer.messageType = pid1;
                        if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error! [ msgsnd() has failed ]\n");
                        if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), getpid(), 0) == -1) fprintf(stderr, "Error [ msgrcv() has failed ]\n");
                    }
                    else {
                        buffer.messageType = pid2;
                        if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error! [ msgsnd() has failed ]\n");
                        if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), getpid(), 0) == -1) fprintf(stderr, "Error! [ msgrcv() has failed ]\n");
                    }
                }
            } while (processNumber == 1 || processNumber == 2);
            strcpy(buffer.messageContent, "EXIT");
            buffer.messageType = pid1;
            if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error! [ msgsnd() has failed ]\n");
            buffer.messageType = pid2;
            if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error! [ msgsnd() has failed ]\n");
            waitpid(pid1, 0, 0);
            waitpid(pid2, 0, 0);
            msgctl(qid, IPC_RMID, 0);
        }
    }
    return 0;
}