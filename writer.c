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
	int index, value;
    int indexPos = MAX;

    semArray A, B, empty;
    empty.n = 0;

    printf("W] writer start\n");


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


    //pamiec krytyczna - POCZATEK
    array(&A, 1);
    A.sems[0] = WRITE;
    VE(A);

    array(&A, 1);
    A.sems[0] = MUTEX;
    array(&B, 1);
    B.sems[0] = READ;
    PE(A, B);

    usleep(100000);


    //odczyt indeksu do wpisywania
    index = *(shmAddr + indexPos * sizeof(int));

    //wpisywanie
    value = getpid() % (123 - 97) + 97;
    *(shmAddr + index * sizeof(int)) = value;
    printf("W] +value[%d]: %c\n", index, value);

    //modyfikacja indeksu do wpisywania
    index++;
    if (index == MAX)
        index = 0;
    *(shmAddr + indexPos * sizeof(int)) = index;

    VE(A);

    A.sems[0] = WRITE;
    PE(A, empty);
    //pamiec krytyczna - KONIEC



    //odlaczanie pamieci dzielonej
    shmdt(shmAddr);

    printf("W] writer end\n");
    return 0;
}

