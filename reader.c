#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>

#include "funkcje.h"

#define MAX_WORD_LEN 30
#define MAX 10
#define MUTEX 0
#define WRITE 1
#define READ 2


int main() {
    key_t shmKey, semKey;
    int shmID, semID;
    int *shmAddr;
    int wordLength, value;
    int indexPos = MAX;

    semArray A, B, empty;
    empty.n = 0;

    printf("R] reader start\n");


    //uzyskanie dosepu do pamieci dzielonej
    shmKey = get_ftok_key('G');
    shmID = get_shm_id(shmKey, (MAX + 1) * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shmAddr = shmat(shmID, NULL, 0);

    //uzyskanie dosepu do semaforow
    semKey = get_ftok_key('M');
    semID = aloc_sem(semKey, 3, IPC_CREAT | 0666);

    //przylaczenie "list" semaforow do glownego
    A.semID = semID;
    B.semID = semID;

    array(&A, 1);  // PE(M;A)
    A.sems[0] = MUTEX;
    array(&B, 1);
    B.sems[0] = WRITE;
    PE(A, B);

    array(&A, 2);  // VE(M, R)
    A.sems[0] = MUTEX;
    A.sems[1] = READ;
    VE(A);


    array(&A, 1);
    A.sems[0] = MUTEX;
    PE(A, empty);
    usleep(500000);

    //czytanie
    wordLength = *(shmAddr + indexPos * sizeof(int));

    printf("R] -values[%d-%d]: ", 0, wordLength - 1);
    for (int index = 0; index < wordLength; index++) {
        value = *(shmAddr + index * sizeof(int));
        printf("%c", value);
    }
    printf("\n");

    VE(A);

//    array(&A, 1);  // PE(R)
//    A.sems[0] = READ;
//    PE(A, empty);

    //odlaczanie pamieci dzielonej
    shmdt(shmAddr);

    printf("R] reader koniec\n");
    return 0;
}

