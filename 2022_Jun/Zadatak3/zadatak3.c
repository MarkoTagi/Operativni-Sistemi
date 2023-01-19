#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#define MSGQ_ID 1337
#define MAX_LEN 255

typedef struct Message {
    long messageType;
    char messageContent[MAX_LEN]
} Message;

int stringToInteger(const char* string) {
    int stringLength = strlen(string);
    int number = 0;
    for (int i = 0; i < stringLength; ++i) {
        int notDigit = !(string[i] >= '0' && string[i] <= '9');
        if (notDigit && (string[i] != '-' || i != 0)) {
            printf("Invalid input! [%s]\n", string);
            return 0;
        }
        int digit = (string[i] != '-') ? string[i] - '0' : -(string[++i] - '0');
        number += (number >= 0) ? digit * pow(10, stringLength - 1 - i) : -(digit * pow(10, stringLength - 1 - i));
    }
    return number;
}

int sumOfDigits(int number) {
    int sum = 0;
    do {
        sum += (number > 0) ? number % 10 : -(number % 10);
    } while ((number /= 10) != 0);
    return sum;
}

int main() {
    int qid = msgget(MSGQ_ID, IPC_CREAT | 0666);
    if (qid == -1) {
        fprintf(stderr, "Error: Could not create a message queue!\n");
        exit(EXIT_FAILURE);
    }
    int pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error: fork() has failed!\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        Message buffer;
        int number;
        for (int i = 0; i < 10; ++i) {
            printf("Enter a number: ");
            scanf("%d", &number);
            sprintf(buffer.messageContent, "%d", number);
            if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error: Could not send the message!\n");
            else printf("Sending...\n");
        }
        wait(NULL);
        msgctl(qid, IPC_RMID, 0);
    }
    else {
        Message buffer;
        for (int i = 0; i < 10; ++i) {
            if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), 0, 0) == -1) fprintf(stderr, "Error: Could not receive the message!\n");
            else {
                printf("Received: %s.\n", buffer.messageContent);
                int number = stringToInteger(buffer.messageContent);
                printf("The sum of its digits: %d.\n", sumOfDigits(number));                      
            }
        }
    }
    return 0;
}