#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX_LEN 255
#define MSGQ_ID 1337

typedef struct Message {
    long messageType;
    char messageContent[MAX_LEN];
} Message;

int main() {
    int qid = msgget(MSGQ_ID, IPC_CREAT | 0666);
    if (qid == -1) {
        fprintf(stderr, "Error: Could not create a message queue!\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error: [1] fork() has failed!\n");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        int execlStatus = execl("./zadatak3a", "./zadatak3a", NULL);
        if (execlStatus == -1) {
            fprintf(stderr, "Error: execl() has failed!\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Error: [2] fork() has failed!\n");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            int execlStatus = execl("./zadatak3b", "./zadatak3b", NULL);
            if (execlStatus == -1) {
                fprintf(stderr, "Error: [zadatak3b] execl() has failed!\n");
                exit(EXIT_FAILURE);
            }
        }
        else {
            Message buffer;
            do {
                printf("Enter something: ");
                fgets(buffer.messageContent, MAX_LEN, stdin);
                int stringLength = strlen(buffer.messageContent);
                buffer.messageContent[--stringLength] = '\0';
                printf("Which process should receive the message? ");
                scanf("%ld", &buffer.messageType);
                fgetc(stdin);
                int sndStatus = msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0);
                if (sndStatus == -1) fprintf(stderr, "Error: Could not send the message!\n");
                else printf("The message was successfully sent.\n");
            } while (buffer.messageType == 1 || buffer.messageType == 2);
            strcpy(buffer.messageContent, "END");
            buffer.messageType = 1;
            if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) {
                fprintf(stderr, "Error: Failed to send termination string!\n"); 
                exit(EXIT_FAILURE);
            }
            buffer.messageType = 2;
            if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) {
                fprintf(stderr, "Error: Failed to send termination string!\n"); 
                exit(EXIT_FAILURE);
            }
            wait(NULL);
            wait(NULL);
            msgctl(qid, IPC_RMID, 0);
        }
    }
    return 0;
}