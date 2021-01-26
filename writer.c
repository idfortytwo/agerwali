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
#define READ 2


int main() {
    key_t shmKey, semKey;
    int shmID, semID;
    int *shared;
    int writeIndex, value;

    int A[1], B[1];


    //uzyskanie dosepu do pamieci dzielonej
    shmKey = get_ftok_key('G');
    shmID = get_shm_id(shmKey, (MAX + 2) * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shared = shmat(shmID, NULL, 0);

    //uzyskanie dosepu do semaforow
    semKey = get_ftok_key('M');
    semID = aloc_sem(semKey, 3, IPC_CREAT | 0666);


    //pamiec krytyczna - POCZATEK
    A[0] = WRITE;
    VE(semID, A, 1);

    A[0] = MUTEX;
    B[0] = READ;
    PE(semID, A, 1, B, 1);

    //wpisywanie
    value = getpid();
    writeIndex = *(shared + MAX * sizeof(int));
    *(shared + writeIndex * sizeof(int)) = value;
    printf("W] pisanie[%d]: %d\n", writeIndex, value);

    //modyfikacja indeksu do wpisywania
    *(shared + MAX * sizeof(int)) = (writeIndex + 1) % MAX;

    VE(semID, A, 1);

    A[0] = WRITE;
    PE(semID, A, 1, B, 0);
    //pamiec krytyczna - KONIEC



    //odlaczanie pamieci dzielonej
    shmdt(shared);

    return 0;
}
