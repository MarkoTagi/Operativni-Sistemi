#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MSGQ_ID 1337
#define MAX_LEN 255

typedef struct Message {
    long messageType;
    char messageContent[MAX_LEN];
} Message;

int main() {
    int qid = msgget(MSGQ_ID, IPC_CREAT | 0666);
    if (qid == -1) {
        fprintf(stderr, "Error! [msgget]\n");
        exit(EXIT_FAILURE);
    }
    Message buffer;
    do {
        if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), 0, 0) == -1) fprintf(stderr, "Error! [Failed to receive the message]\n");
        else if (strcmp(buffer.messageContent, "END") != 0) {
            printf("Received: %s\n", buffer.messageContent);
            buffer.messageType = 1;
            strcpy(buffer.messageContent, "I'm done!");
            msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0);
        }
    } while (strcmp(buffer.messageContent, "END") != 0);
    return 0;
}