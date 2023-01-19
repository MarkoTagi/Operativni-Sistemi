#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "bad usage: %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int qid = msgget(MSGQ_ID, IPC_CREAT | 0666);
    if (qid == -1) {
        fprintf(stderr, "Error! [msgget()]\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error! [fork()]\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        FILE* filep = fopen(argv[1], "r");
        if (filep == NULL) {
            fprintf(stderr, "Parent> Error! [fopen()]\n");
            exit(EXIT_FAILURE);
        }
        Message buffer;
        int lineCount = 0;
        while (fgets(buffer.messageContent, MAX_LEN, filep) != NULL) {
            ++lineCount;
            int lineLength = strlen(buffer.messageContent);
            buffer.messageContent[--lineLength] = '\0';
            buffer.messageType = pid;
            if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) {
                fprintf(stderr, "Parent> Error! [msgsnd()]\n");
                exit(EXIT_FAILURE);
            }
            // else {
            //     printf("Parent> The message was successfully sent! [%s]\n", buffer.messageContent); fflush(stdout);
            // }
            if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), getpid(), 0) == -1) fprintf(stderr, "Parent> Error! [Failed to receive info about uppercase letters]\n");
            char countNumber[MAX_LEN];
            strcpy(countNumber, buffer.messageContent);
            if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), getpid(), 0) == -1) fprintf(stderr, "Parent> Error! [Failed to receive info about the line]\n");
            printf("Line %s: Number of uppercase letters [%s].\n", buffer.messageContent, countNumber); fflush(stdout);
        }
        strcpy(buffer.messageContent, "END");
        buffer.messageType = pid;
        msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0);
        wait(NULL);
        msgctl(qid, IPC_RMID, 0);
        printf("END\n"); fflush(stdout);
        fclose(filep);
    }
    else {
        Message buffer;
        int lineCount = 0;
        do {
            if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), getpid(), 0) == -1) {
                fprintf(stderr, "Child> Error! [msgrcv()]\n");
                exit(EXIT_FAILURE);
            }
            else if (strcmp(buffer.messageContent, "END") != 0) {
                ++lineCount;
                int i = 0, count = 0;
                while (buffer.messageContent[i] != '\0') {
                    int isUppercase = buffer.messageContent[i] >= 'A' && buffer.messageContent[i] <= 'Z';
                    if (isUppercase) ++count;
                    ++i;
                }
                char countBuffer[MAX_LEN]; sprintf(countBuffer, "%d", count);
                char lineBuffer[MAX_LEN]; sprintf(lineBuffer, "%d", lineCount);
                strcpy(buffer.messageContent, countBuffer);
                buffer.messageType = getppid();
                if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Child> Error! [Failed to send info about uppercase letters]\n");
                // else {
                //     printf("Child> Sending uppercase letters info...\n"); fflush(stdout);
                // }
                strcpy(buffer.messageContent, lineBuffer);
                if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Child> Error! [Failed to send info about the line]\n");
                // else {
                //     printf("Child> Sending line info...\n"); fflush(stdout);
                // }
            }
        } while (strcmp(buffer.messageContent, "END") != 0);
        // printf("Child is exiting...\n"); fflush(stdout);
    }
    return 0;
}