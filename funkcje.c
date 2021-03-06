#include "funkcje.h"

int get_ftok_key(char key) {
    int ftok_key;

    ftok_key = ftok(".", key);
    if (ftok_key == -1) {
        printf("error ftok for key %c\n", key);
        perror("err: ");
        exit(1);
    }

    return ftok_key;
}

int get_shm_id(key_t shm_key, size_t size, int shmflg) {
    int shm_id;

    shm_id = shmget(shm_key, size, shmflg); //znajdowanie pam. dz.
    if (shm_id == -1) {
        printf("get_shm_id(%d) error (pam. dz.)\n", shm_key);
        perror("err: ");
        shmdt(&shm_id);
        exit(1);
    }

    return shm_id;
};



int aloc_sem(key_t klucz, int number, int flagi) {
    int semID;
    if ((semID = semget(klucz, number, flagi)) == -1) {
        perror("Blad semget (alokujSemafor): ");
        exit(1);
    }
    return semID;
}

int free_sem(int semID, int number) {
    return semctl(semID, number, IPC_RMID, NULL);
}

void init_sem(int semID, int number, int val) {
    if (semctl(semID, number, SETVAL, val) == -1) {
        perror("Blad semctl (inicjalizujSemafor): ");
        exit(1);
    }
}


int PE(int semID, int* A, int lenA, int* B, int lenB) {
    struct sembuf operacje[2];

    int i = 0;
    for (; i < lenA; i++) {
        operacje[i].sem_num = A[i];
        operacje[i].sem_op = -1;
        operacje[i].sem_flg = SEM_UNDO;
    }
    for (int j = 0; j < lenA; j++) {
        operacje[i].sem_num = B[j];
        operacje[i].sem_op = 0;
        operacje[i].sem_flg = SEM_UNDO;
        i++;
    }

    if (semop(semID, operacje, lenA + lenB) == -1) {
        perror("Blad semop (PE):");
        return -1;
    }

    return 1;
}

int VE(int semID, int* A, int lenA) {
    struct sembuf operacje[2];

    for (int i = 0; i < lenA; i++) {
        operacje[i].sem_num = A[i];
        operacje[i].sem_op = 1;
        operacje[i].sem_flg = SEM_UNDO;
    }

    if (semop(semID, operacje, lenA) == -1) {
        perror("Blad semop (VE):");
        return -1;
    }

    return 1;
}