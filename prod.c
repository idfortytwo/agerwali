#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "funkcje.h"


struct bufor {
    long mtype;
    int mvalue;
};

#define MAX 10
#define MAX2 12
#define PELNY 2
#define PUSTY 1


int main() {
    key_t msg_key, shm_key, sem_key;
    int msg_id, shm_id, sem_id;
    int *shm_addr;
	int writeIndex, value;
    struct bufor komunikat;

    printf("P] producent start\n");


    //uzyskanie dosepu do kolejki komunikatow
    msg_key = get_ftok_key('R');
    msg_id = get_msg_id(msg_key, 0666);

    //uzyskanie dosepu do pamieci dzielonej
	shm_key = get_ftok_key('G');
    shm_id = get_shm_id(shm_key, MAX2 * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shm_addr = shmat(shm_id, NULL, 0);

    //uzyskanie dosepu do semaforow
    sem_key = get_ftok_key('M');
    sem_id = aloc_sem(sem_key, 2, IPC_CREAT | 0666);


    //czekamy na komunikat od konsumenta
    if (msgrcv(msg_id, &komunikat, sizeof(komunikat.mvalue), PUSTY, MSG_NOERROR) == -1)
        msgctl(msg_id, IPC_RMID, NULL);


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


    //wysylanie komunikatow do konsumentow
    komunikat.mtype = PELNY;

    if (msgsnd(msg_id, &komunikat, sizeof(komunikat.mvalue), 0) == -1) {
        printf("P] sending error val %d (prod)\n", komunikat.mvalue);
        shmctl(shm_id, IPC_RMID, NULL);
        msgctl(msg_id, IPC_RMID, NULL);
        exit(1);
    }


    //odlaczanie pamieci dzielonej
    shmdt(shm_addr);

//    printf("P] producent koniec\n");
    return 0;
}

