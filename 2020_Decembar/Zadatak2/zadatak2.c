#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#define MAX_LEN 40

sem_t even, odd;

void* oddThread(void*);

int main() {
    FILE* filep = fopen("izlaz.txt", "w");
    if (filep == NULL) {
        fprintf(stderr, "Could not open file! (izlaz.txt)\n");
        exit(EXIT_FAILURE);
    }
    pthread_t oddThread_id;
    sem_init(&odd, 0, 0);
    sem_init(&even, 0, 1);
    pthread_create(&oddThread_id, NULL, oddThread, filep);
    for (int i = 0; i <= 20; ++i) {
        char number[5];
        sem_wait(&even);
        sprintf(number, "(%d)", i * 2);
        fputs(number, filep);
        fputs(" ", filep);
        sem_post(&odd);
    }
    fputs("\n", filep);
    sem_destroy(&even);
    sem_destroy(&odd);
    fclose(filep);
    return 0;
}

void* oddThread(void* arg) {
    FILE* filep = (FILE*)arg;
    for(int i = 0; i < 40; ++i) {
        char number[5];
        sem_wait(&odd);
        sprintf(number, "(%d)", ++i);
        fputs(number, filep);
        fputs(" ", filep);
        sem_post(&even);
    }
    return NULL;
}
