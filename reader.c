#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>

#include "funkcje.h"


#define MAX 10
#define MUTEX 0
#define WRITE 1
#define READ 2


int main() {
    key_t shmKey, semKey;
    int shmID, semID;
    int *shmAddr;
    int readIndex, value;
    int i_readIndex = MAX;

    int A[2], B[1];

//    printf("R] reader start\n");


    //uzyskanie dosepu do pamieci dzielonej
    shmKey = get_ftok_key('G');
    shmID = get_shm_id(shmKey, (MAX + 2) * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shmAddr = shmat(shmID, NULL, 0);

    //uzyskanie dosepu do semaforow
    semKey = get_ftok_key('M');
    semID = aloc_sem(semKey, 3, IPC_CREAT | 0666);

    A[0] = MUTEX;
    B[0] = WRITE;
    PE(semID, A, 1, B, 1);

    A[0] = MUTEX;
    A[1] = READ;
    VE(semID, A, 2);

    //czytanie
    readIndex = *(shmAddr + i_readIndex * sizeof(int));
    printf("R] -values[0-%d]: ", readIndex);
    for (int i = 0; i < readIndex; i++)
        printf("%d ", *(shmAddr + i * sizeof(int)));
    printf("\n");


    A[0] = READ;
    PE(semID, A, 1, B, 0);


    //odlaczanie pamieci dzielonej
    shmdt(shmAddr);

//    printf("R] reader koniec\n");
    return 0;
}
