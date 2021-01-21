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
    int *shmAddr;
	int writeIndex, value;
    int i_writeIndex = MAX + 2;

    semArray A, B, empty;
    empty.n = 0;

    printf("P] producent start\n");


    //uzyskanie dosepu do pamieci dzielonej
	shmKey = get_ftok_key('G');
    shmID = get_shm_id(shmKey, (MAX + 3) * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shmAddr = shmat(shmID, NULL, 0);

    //uzyskanie dosepu do semaforow
    semKey = get_ftok_key('M');
    semID = aloc_sem(semKey, 3, IPC_CREAT | 0666);
//    semID = 5;
    A.semID = semID;
    B.semID = semID;



    //pamiec krytyczna - POCZATEK
    array(&A, 1);
    A.sems[0] = WRITE;
    VE(A);

    A.sems[0] = MUTEX;
//    B.n = 1;
    array(&B, 1);
//    B.sems = (int*) malloc(sizeof(int) * 1);
    B.sems[0] = READ;
    PE(A, B);
    sleep(1);


    //odczyt indeksu do wpisywania
    writeIndex = *(shmAddr + i_writeIndex * sizeof(int));

    //wpisywanie
    value = getpid();
    *(shmAddr + writeIndex * sizeof(int)) = value;
    printf("P] +value[%d]: %d\n", writeIndex, value);

    //modyfikacja indeksu do wpisywania
    writeIndex++;
    if (writeIndex == MAX)
        writeIndex = 0;
    *(shmAddr + i_writeIndex * sizeof(int)) = writeIndex;

    VE(A);

    A.sems[0] = WRITE;
    B.n = 0;
    PE(A, empty);
    //pamiec krytyczna - KONIEC



    //odlaczanie pamieci dzielonej
    shmdt(shmAddr);

//    printf("P] producent koniec\n");
    return 0;
}

