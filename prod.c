#include <stdio.h>
#include <unistd.h>
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
	int writeIndex, value;

    printf("P] producent start\n");


    //uzyskanie dosepu do pamieci dzielonej
	shmKey = get_ftok_key('G');
    shmID = get_shm_id(shmKey, (MAX + 3) * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shmAddr = shmat(shmID, NULL, 0);

    //uzyskanie dosepu do semaforow
    semKey = get_ftok_key('M');
    semID = aloc_sem(semKey, 2, IPC_CREAT | 0666);


    //pamiec krytyczna - POCZATEK
    wait_sem(semID, 0, 0);

    //odczyt indeksu do wpisywania
    writeIndex = *(shmAddr + 10 * sizeof(int));

    //wpisywanie
    value = getpid();
    *(shmAddr + writeIndex * sizeof(int)) = value;
    printf("P] +value[%d]: %d\n", writeIndex, value);

    //modyfikacja indeksu do wpisywania
    writeIndex++;
    if (writeIndex == MAX)
        writeIndex = 0;
    *(shmAddr + 10 * sizeof(int)) = writeIndex;

    //pamiec krytyczna - KONIEC
    signal_sem(semID, 0);


    //odlaczanie pamieci dzielonej
    shmdt(shmAddr);

//    printf("P] producent koniec\n");
    return 0;
}

