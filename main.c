#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/shm.h>

#include "funkcje.h"


#define P 50    //liczba procesow prod i kons
#define MAX 10  //rozmiar buforu
#define MAX2 12 //dwa pola na indeksy zapis/odczyt


int kek() {
    key_t shm_key, sem_key;
    int shm_id, sem_id;

    printf("M] main start\n");

    //tworzymy pamiec dzielona
    shm_key = get_ftok_key('G');
	shm_id = get_shm_id(shm_key, MAX2 * sizeof(int), IPC_CREAT | IPC_EXCL | 0666);


    //tworzymy i inicjujemy semafory dla pamieci krytycznej
    sem_key = get_ftok_key('M');
    sem_id = aloc_sem(sem_key, 2, IPC_CREAT | IPC_EXCL | 0666);
    for (int sem_num = 0; sem_num < 2; sem_num++)
        init_sem(sem_id, sem_num, 1);

    //uruchamiamy producentow
    for (int i = 0; i < P; i++) {
        switch (fork()) {
            case -1:
                perror("M] Blad fork (mainprog)");
                exit(2);
            case 0:
                execl("./prod", "prod", NULL);
        }
    }

    //uruchamiamy konsumentow
    for (int i = 0; i < P; i++) {
        switch (fork()) {
            case -1:
                printf("M] Blad fork (mainprog)\n");
                exit(2);
            case 0:
                execl("./kons", "kons", NULL);
        }
    }

    for (int i = 0; i < 2 * P; i++)
        wait(NULL);

    // zwalnianie zasobow
    free_sem(sem_id, 2);
	shmctl(shm_id, IPC_RMID, NULL);

    printf("M] main koniec\n");
    return 0;
}

int main() {
    kek();
}