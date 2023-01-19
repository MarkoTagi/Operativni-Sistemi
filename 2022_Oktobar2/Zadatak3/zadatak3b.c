#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
        fprintf(stderr, "Process 2 Error: Could not create a message queue!\n");
        exit(EXIT_FAILURE);
    }
    Message buffer;
    do {
        int rcvStatus = msgrcv(qid, &buffer, sizeof(buffer.messageContent), 2, 0);
        if (rcvStatus == -1) fprintf(stderr, "Process 2 Error: Could not receive the message!\n");
        else if (strcmp(buffer.messageContent, "END") != 0) {
            int stringLength = strlen(buffer.messageContent);
            printf("Received: [%s]\n", buffer.messageContent);
            for (int i = 0; i < stringLength; ++i) buffer.messageContent[i] -= (buffer.messageContent[i] >= 'a' && buffer.messageContent[i] <= 'z') ? 32 : 0;
            printf("Converted: [%s]\n", buffer.messageContent);
        } 
    } while (strcmp(buffer.messageContent, "END") != 0);
    return 0;
}