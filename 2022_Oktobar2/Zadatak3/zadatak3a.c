#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        fprintf(stderr, "Process 1 Error: Could not create a message queue!\n");
        exit(EXIT_FAILURE);
    }
    Message buffer;
    do {
        int bytesReceived = msgrcv(qid, &buffer, sizeof(buffer.messageContent), 1, 0);
        if (bytesReceived == -1) fprintf(stderr, "Process 1 Error: Could not received the message!\n");
        else if (strcmp(buffer.messageContent, "END") != 0) printf("Received: [%s]\nSize: %d.\n", buffer.messageContent, bytesReceived);
    } while (strcmp(buffer.messageContent, "END") != 0);
    return 0;
}