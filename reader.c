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

    semArray A, B, empty;
    empty.n = 0;

    printf("R] reader start\n");


    //uzyskanie dosepu do pamieci dzielonej
    shmKey = get_ftok_key('G');
    shmID = get_shm_id(shmKey, (MAX + 2) * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shmAddr = shmat(shmID, NULL, 0);

    //uzyskanie dosepu do semaforow
    semKey = get_ftok_key('M');
    semID = aloc_sem(semKey, 3, IPC_CREAT | 0666);

    //przylaczenie "list" semaforow do glownego
    A.semID = semID;
    B.semID = semID;

    array(&A, 1);
    A.sems[0] = MUTEX;
    array(&B, 1);
    B.sems[0] = WRITE;
    PE(A, B);

    array(&A, 2);
    A.sems[0] = MUTEX;
    A.sems[1] = READ;
    VE(A);

    //pamiec krytyczna - POCZATEK
    array(&A, 1);
    A.sems[0] = MUTEX;
    PE(A, empty);

    //czytanie
    readIndex = *(shmAddr + i_readIndex * sizeof(int));
    value = *(shmAddr + readIndex * sizeof(int));
    printf("R] -value[%d]: %d\n", readIndex, value);

    //modyfikacja indeksu do odczytu
    readIndex++;
    if (readIndex == MAX)
        readIndex = 0;
    *(shmAddr + i_readIndex * sizeof(int)) = readIndex;

    VE(A);
    //pamiec krytyczna - KONIEC


    //odlaczanie pamieci dzielonej
    shmdt(shmAddr);

    printf("R] reader koniec\n");
    return 0;
}

