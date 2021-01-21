#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "funkcje.h"


#define MAX 10
#define MAX2 12

int main() {
    key_t shm_key, sem_key;
    int shm_id, sem_id;
    int *shm_addr;
    int readIndex, value;

    printf("K] konsument start\n");


    //uzyskanie dosepu do pamieci dzielonej
    shm_key = get_ftok_key('G');
    shm_id = get_shm_id(shm_key, MAX2 * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shm_addr = shmat(shm_id, NULL, 0);

    //uzyskanie dosepu do semaforow
    sem_key = get_ftok_key('M');
    sem_id = aloc_sem(sem_key, 2, IPC_CREAT | 0666);


    //pamiec krytyczna - POCZATEK
    wait_sem(sem_id, 1, 0);

    //odczyt indeksu do odczytu
    readIndex = *(shm_addr + 11 * sizeof(int));

    //odczyt
    value = *(shm_addr + readIndex * sizeof(int));
    printf("K] -value[%d]: %d\n", readIndex, value);

    //modyfikacja indeksu do odczytu
    readIndex++;
    if (readIndex == MAX)
        readIndex = 0;
    *(shm_addr + 11 * sizeof(int)) = readIndex;

    //pamiec krytyczna - KONIEC
    signal_sem(sem_id, 1);


    //odlaczanie pamieci dzielonej
    shmdt(shm_addr);

//    printf("K] konsument koniec\n");
    return 0;
}

