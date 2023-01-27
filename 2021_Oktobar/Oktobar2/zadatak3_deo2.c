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
        fprintf(stderr, "Error! [ msgget() has failed ]\n");
        exit(EXIT_FAILURE);
    }
    Message buffer;
    do {
        if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), getpid(), 0) == -1) fprintf(stderr, "Error! [ msgrcv() has failed ]\n");
        else if (strcmp(buffer.messageContent, "EXIT") != 0) {
            int messageLength = 0;
            while (buffer.messageContent[messageLength] != '\0') ++messageLength;
            printf("Received a message: %s [ Message length = %d ]\n", buffer.messageContent, messageLength);
            strcpy(buffer.messageContent, "Finished reading!");
            buffer.messageType = getppid();
            if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error! [ msgsnd() has failed ]\n");
        }
    } while (strcmp(buffer.messageContent, "EXIT") != 0);
    return 0;
}