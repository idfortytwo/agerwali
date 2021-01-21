#include <stdio.h>
#include <unistd.h>
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
	int writeIndex, value;

    printf("P] producent start\n");


    //uzyskanie dosepu do pamieci dzielonej
	shm_key = get_ftok_key('G');
    shm_id = get_shm_id(shm_key, MAX2 * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shm_addr = shmat(shm_id, NULL, 0);

    //uzyskanie dosepu do semaforow
    sem_key = get_ftok_key('M');
    sem_id = aloc_sem(sem_key, 2, IPC_CREAT | 0666);


    //pamiec krytyczna - POCZATEK
    wait_sem(sem_id, 0, 0);

    //odczyt indeksu do wpisywania
    writeIndex = *(shm_addr + 10 * sizeof(int));

    //wpisywanie
    value = getpid();
    *(shm_addr + writeIndex * sizeof(int)) = value;
    printf("P] +value[%d]: %d\n", writeIndex, value);

    //modyfikacja indeksu do wpisywania
    writeIndex++;
    if (writeIndex == MAX)
        writeIndex = 0;
    *(shm_addr + 10 * sizeof(int)) = writeIndex;

    //pamiec krytyczna - KONIEC
    signal_sem(sem_id, 0);


    //odlaczanie pamieci dzielonej
    shmdt(shm_addr);

//    printf("P] producent koniec\n");
    return 0;
}

