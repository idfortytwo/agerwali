#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "funkcje.h"


#define MAX 10
#define MUTEX 0
#define WRITE 1


int main() {
    key_t shmKey, semKey;
    int shmID, semID;
    int *shmAddr;
    int readIndex, value, readCount;
    int i_readIndex = MAX, i_readCount = MAX + 1;

    printf("K] konsument start\n");


    //uzyskanie dosepu do pamieci dzielonej
    shmKey = get_ftok_key('G');
    shmID = get_shm_id(shmKey, (MAX + 3) * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shmAddr = shmat(shmID, NULL, 0);

    //uzyskanie dosepu do semaforow
    semKey = get_ftok_key('M');
    semID = aloc_sem(semKey, 2, IPC_CREAT | 0666);



    //pamiec krytyczna - POCZATEK
    wait_sem(semID, MUTEX, 0);

    readCount = *(shmAddr + i_readCount * sizeof(int));
    readCount++;
    if (readCount == 1)
        wait_sem(semID, WRITE, 0);

    signal_sem(semID, MUTEX);


    //czytanie
    readIndex = *(shmAddr + i_readIndex * sizeof(int));

    value = *(shmAddr + readIndex * sizeof(int));
    printf("K] -value[%d]: %d\n", readIndex, value);

    //modyfikacja indeksu do odczytu
    readIndex++;
    if (readIndex == MAX)
        readIndex = 0;
    *(shmAddr + 11 * sizeof(int)) = readIndex;


    wait_sem(semID, MUTEX, 0);
    readCount--;
    if (readCount == 0)
        signal_sem(semID, WRITE);
    signal_sem(semID, MUTEX);
    //pamiec krytyczna - KONIEC



    //odlaczanie pamieci dzielonej
    shmdt(shmAddr);

//    printf("K] konsument koniec\n");
    return 0;
}

