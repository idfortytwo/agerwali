#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/shm.h>

#include "funkcje.h"


#define P 15
#define MAX 10
#define MUTEX 0
#define WRITE 1
#define READ 2


key_t shmKey, semKey;
int shmID, semID;

void handler(int s) {
    if (s == 2) {
        free_sem(semID, 2);
        shmctl(shmID, IPC_RMID, NULL);
    }
    exit(1);
}

int main() {
    printf("M] main start\n");

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    //tworzymy pamiec dzielona
    shmKey = get_ftok_key('G');
    shmID = get_shm_id(shmKey, (MAX + 2) * sizeof(int), IPC_CREAT | IPC_EXCL | 0666);

    //tworzymy i inicjujemy semafory
    semKey = get_ftok_key('M');
    semID = aloc_sem(semKey, 3, IPC_CREAT | IPC_EXCL | 0666);
    init_sem(semID, MUTEX, 1);
    init_sem(semID, WRITE, 0);
    init_sem(semID, READ, 0);

    //uruchamiamy pisarzy
    for (int i = 0; i < P; i++) {
        switch (fork()) {
            case -1:
                perror("M] Blad fork (mainprog)");
                exit(2);
            case 0:
                execl("./writer", "writer", NULL);
        }
    }

    //uruchamiamy czytelnikow
    for (int i = 0; i < P; i++) {
        switch (fork()) {
            case -1:
                printf("M] Blad fork (mainprog)\n");
                exit(2);
            case 0:
                execl("./reader", "reader", NULL);
        }
    }

    //czekamy na procesy
    for (int i = 0; i < 2 * P; i++)
        wait(NULL);

    //zwalnianie zasobow
    free_sem(semID, 2);
	shmctl(shmID, IPC_RMID, NULL);

    printf("M] main koniec\n");
    return 0;
}