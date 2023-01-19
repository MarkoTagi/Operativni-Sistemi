#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#define MSGQ_ID 1337
#define MAX_LEN 255

typedef struct Message {
    long messageType;
    char messageContent[MAX_LEN];
} Message;

int main() {
    int qid = msgget(MSGQ_ID, IPC_CREAT | 0666);
    if (qid == -1) {
        fprintf(stderr, "Error: [Could not create a message queue!]\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error: [fork() has failed!]\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        srand(time(0));
        Message buffer;
        int count = rand() % 20 + 1;
        printf("%d random numbers to be generated and sent...\n", count);
        for (int i = 0; i < count; ++i) {
            int randomNumber = rand() % 99 + 1;
            sprintf(buffer.messageContent, "%d", randomNumber);
            if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error: [Could not send the number (%d)!]\n", randomNumber);
            else printf ("Successfuly sent number [%d]!\n", randomNumber);
        }
        strcpy(buffer.messageContent, "-1");
        if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error: [Termination string was not sent!]\n");
        wait(NULL);
        msgctl(qid, IPC_RMID, 0);
    }
    else {
        Message buffer;
        do {
            if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), 0, 0) == -1) fprintf(stderr, "Error: [Could not receive the number!]\n");
            else if (strcmp(buffer.messageContent, "-1") != 0) printf("Received number [%s].\n", buffer.messageContent);
        } while (strcmp(buffer.messageContent, "-1") != 0);
    }
    return 0;
}