#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void* generatingThread(void* arg) {
    int* buffer = (int*)arg;
    int randomNumber = (rand() % 100) + 100;
    printf("Generating_Thread> Number (%d) was generated.\n", randomNumber);
    buffer[0] = randomNumber;
    randomNumber = (rand() % 100) + 100;
    printf("Generating_Thread> Number (%d) was generated.\n", randomNumber);
    buffer[1] = randomNumber;
}

int main() {
    int buffer[2];
    srand(time(0));
    pthread_t generatingThread_id;
    int i = 0;
    do {
        pthread_create(&generatingThread_id, NULL, generatingThread, buffer);
        pthread_join(generatingThread_id, NULL);
        if (buffer[0] == buffer[1]) printf("Main> The numbers are equal (%d, %d).\n", buffer[0], buffer[1]);
        else printf("Main> The numbers are not equal (%d, %d).\n", buffer[0], buffer[1]);
        ++i;
    } while (i < 10);
    printf("Main> Exiting...\n");
    return 0;
}