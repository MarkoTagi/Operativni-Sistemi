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
    char messageContent[MAX_LEN];
} Message;

typedef struct Student {
    int indexNo;
    char fullName[MAX_LEN];
} Student;

int stringToInteger(const char* string) {
    int stringLength = 0;
    while (string[stringLength] != '\0') ++stringLength;
    int theNumber = 0;
    for (int i = 0; i < stringLength; ++i) {
        int notDigit = !(string[i] >= '0' && string[i] <= '9');
        if (notDigit && (string[i] != '-' || i != 0)) return 0;
        int aDigit = (string[i] != '-') ? string[i] - '0' : -(string[++i] - '0');
        theNumber += (theNumber >= 0) ? aDigit * pow(10, stringLength - 1 -i) : -(aDigit * pow(10, stringLength - 1 - i));
    }
    return theNumber;
}

void selectionSort(Student inputArray[], int arraySize) {
    for (int i = 0; i < (arraySize - 1); ++i) {
        int minIndex = i;
        for (int j = i + 1; j < arraySize; ++j) minIndex = (inputArray[j].indexNo < inputArray[minIndex].indexNo) ? j : minIndex;
        if (minIndex != i) {
            Student temp;
            temp.indexNo = inputArray[i].indexNo;
            strcpy(temp.fullName, inputArray[i].fullName);
            inputArray[i].indexNo = inputArray[minIndex].indexNo;
            strcpy(inputArray[i].fullName, inputArray[minIndex].fullName);
            inputArray[minIndex].indexNo = temp.indexNo;
            strcpy(inputArray[minIndex].fullName, temp.fullName);
        }
    }
}

void printStudent(Student student) {
    printf("Student:\n\tIndex number: %d.\n\tFull name: %s.\n", student.indexNo, student.fullName);
}


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
        Message buffer;
        int studentCount;
        printf("How many students are you entering? ");
        scanf("%d", &studentCount);
        fgetc(stdin);
        sprintf(buffer.messageContent, "%d", studentCount);
        if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error: [Could not send the information!]\n");
        for (int i = 0; i < studentCount; ++i) {
            printf("Student #%d:\n", i + 1);
            printf("\tEnter index number: ");
            fgets(buffer.messageContent, MAX_LEN, stdin);
            int indexLength = strlen(buffer.messageContent);
            buffer.messageContent[--indexLength] = '\0';
            buffer.messageType = 1;
            if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error: [Could not send the information!]\n");
            else printf("Sending information...\n");
            printf("\tEnter full name: ");
            fgets(buffer.messageContent, MAX_LEN, stdin);
            int nameLength = strlen(buffer.messageContent);
            buffer.messageContent[--nameLength] = '\0';
            buffer.messageType = 2;
            if (msgsnd(qid, &buffer, sizeof(buffer.messageContent), 0) == -1) fprintf(stderr, "Error: [Could not send the information!]\n");
            else printf("Sending information...\n");
        }
        wait(NULL);
        msgctl(qid, IPC_RMID, 0);
    }
    else {
        Message buffer;
        if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), 0, 0) == -1) {
            fprintf(stderr, "Error: [Could not receive student count!]\n");
            exit(EXIT_FAILURE);
        }
        int studentCount = stringToInteger(buffer.messageContent);
        Student* students = malloc(studentCount * sizeof(Student));
        for (int i = 0; i < studentCount; ++i) {
            if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), 1, 0) == -1) fprintf(stderr, "Error: [Could not receive student index number!]\n");
            else {
                int indexNo = stringToInteger(buffer.messageContent);
                students[i].indexNo = indexNo;
            } 
            if (msgrcv(qid, &buffer, sizeof(buffer.messageContent), 2, 0) == -1) fprintf(stderr, "Error: [Could not receive students full name!]\n");
            else strcpy(students[i].fullName, buffer.messageContent);
        }
        selectionSort(students, studentCount);
        for(int i = 0; i < studentCount; ++i) printStudent(students[i]);
        free(students);
    }
    return 0;
}