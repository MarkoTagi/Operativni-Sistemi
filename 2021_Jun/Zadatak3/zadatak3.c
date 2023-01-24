#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#define SHARED_MEMORY_SIZE 1024

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short* array;
    struct seminfo* __buf;
};

int main() {
    key_t sharedMemoryKey = ftok("/tmp", 'a');
    if (sharedMemoryKey == -1) {
        perror("Failed to create shared memory key!\nReason");
        exit(EXIT_FAILURE);
    }
    int sharedMemoryId = shmget(sharedMemoryKey, SHARED_MEMORY_SIZE * sizeof(char), IPC_CREAT | 0666);
    if (sharedMemoryId == -1) {
        perror("Failed to create shared memory!\nReason");
        exit(EXIT_FAILURE);
    }
    key_t writeMutexKey = ftok("/tmp", 'b');
    if (writeMutexKey == -1) {
        perror("Failed to create write mutex key!\nReason");
        exit(EXIT_FAILURE);
    }
    int writeMutexId = semget(writeMutexKey, 1, IPC_CREAT | 0666);
    if (writeMutexId == -1) {
        perror("Failed to create write mutex!\nReason");
        exit(EXIT_FAILURE);
    }
    key_t readSemaphoreKey = ftok("/tmp", 'c');
    if (readSemaphoreKey == -1) {
        perror("Failed to create read semaphore key!\nReason");
        exit(EXIT_FAILURE);
    }
    int readSemaphoreId = semget(readSemaphoreKey, 1, IPC_CREAT | 0666);
    if (readSemaphoreId == -1) {
        perror("Failed to create read semaphore!\nReason");
        exit(EXIT_FAILURE);
    }
    union semun semaphoreUnion;
    semaphoreUnion.val = 1;
    if (semctl(writeMutexId, 0, SETVAL, semaphoreUnion) == -1) {
        perror("Failed to set write mutex value!\nReason");
        exit(EXIT_FAILURE);
    }
    semaphoreUnion.val = 0;
    if (semctl(readSemaphoreId, 0, SETVAL, semaphoreUnion) == -1) {
        perror("Failed to set read semaphore value!\nReason");
        exit(EXIT_FAILURE);
    }
    struct sembuf semaphoreWait = { 0, -1, 0 }, semaphorePost = { 0, 1, 0 };
    int pid = fork();
    if (pid == -1) {
        perror("Failed to fork!\nReason");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        char* sharedMemory_userInput = (char*)shmat(sharedMemoryId, NULL, 0);
        if (sharedMemory_userInput == (void*) -1) {
            perror("Parent> Failed to map shared memory!\nReason");
            exit(EXIT_FAILURE);
        }
        do {
            semop(writeMutexId, &semaphoreWait, 1);
            printf("Waiting for user input: ");
            fgets(sharedMemory_userInput, SHARED_MEMORY_SIZE * sizeof(char), stdin);
            int inputLength = strlen(sharedMemory_userInput);
            sharedMemory_userInput[--inputLength] = '\0';
            semop(readSemaphoreId, &semaphorePost, 1);
        } while (strcmp(sharedMemory_userInput, "QUIT") != 0);
        shmdt(sharedMemory_userInput);
        wait(NULL);
        printf("Parent> Exiting...\n");
    }
    else {
        char* sharedMemory_userInput = (char*)shmat(sharedMemoryId, NULL, 0);
        if (sharedMemory_userInput == (void*) -1) {
            perror("Child> Failed to map shared memory!\nReason");
            exit(EXIT_FAILURE);
        }
        do {
            semop(readSemaphoreId, &semaphoreWait, 1);
            if (strcmp(sharedMemory_userInput, "QUIT") != 0) printf("Received: [%s]\n", sharedMemory_userInput);
            semop(writeMutexId, &semaphorePost, 1);
        } while (strcmp(sharedMemory_userInput, "QUIT") != 0);
        shmdt(sharedMemory_userInput);
        printf("Child> Exiting...\n");
    }
    semctl(writeMutexId, 0, IPC_RMID);
    semctl(readSemaphoreId, 0, IPC_RMID);
    shmctl(sharedMemoryId, IPC_RMID, NULL);
    return 0;
}