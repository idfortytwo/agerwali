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
    key_t shm_key, sem_key;
    int shm_id, sem_id;
    int *shm_addr;
    int read_index, value, read_count;
    int i_read_index = MAX, i_read_count = MAX + 1;

    printf("K] konsument start\n");


    //uzyskanie dosepu do pamieci dzielonej
    shm_key = get_ftok_key('G');
    shm_id = get_shm_id(shm_key, (MAX + 3) * sizeof(int), 0666);

    //przylaczenie pamieci dzielonej
    shm_addr = shmat(shm_id, NULL, 0);

    //uzyskanie dosepu do semaforow
    sem_key = get_ftok_key('M');
    sem_id = aloc_sem(sem_key, 2, IPC_CREAT | 0666);



    //pamiec krytyczna - POCZATEK
    wait_sem(sem_id, 1, 0);

    wait_sem(sem_id, MUTEX, 0);

    read_count = *(shm_addr + i_read_count * sizeof(int));
    read_count++;
    if (read_count == 1)
        wait_sem(sem_id, WRITE, 0);

    signal_sem(sem_id, MUTEX);


    //czytanie
    read_index = *(shm_addr + i_read_index * sizeof(int));

    value = *(shm_addr + read_index * sizeof(int));
    printf("K] -value[%d]: %d\n", read_index, value);

    //modyfikacja indeksu do odczytu
    read_index++;
    if (read_index == MAX)
        read_index = 0;
    *(shm_addr + 11 * sizeof(int)) = read_index;


    wait_sem(sem_id, MUTEX, 0);
    read_count--;
    if (read_count == 0)
        signal_sem(sem_id, WRITE);
    signal_sem(sem_id, MUTEX);


    //pamiec krytyczna - KONIEC
    signal_sem(sem_id, 1);



    //odlaczanie pamieci dzielonej
    shmdt(shm_addr);

//    printf("K] konsument koniec\n");
    return 0;
}

