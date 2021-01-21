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

int get_msg_id(key_t msg_key, int msgflg) {
    int msg_id;

    msg_id = msgget(msg_key, msgflg); //tworzenie kk
    if (msg_id == -1) {
        printf("get_msg_id(%d) error (k. k.)\n", msg_key);
        perror("err: ");
        exit(1);
    }

    return msg_id;
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

int wait_sem(int semID, int number, int flags) {
    struct sembuf operacje[1];
    operacje[0].sem_num = number;
    operacje[0].sem_op = -1;
    operacje[0].sem_flg = 0 | flags; //SEM_UNDO;

    if (semop(semID, operacje, 1) == -1) {
        perror("Blad semop (waitSemafor)");
        return -1;
    }
//    sleep(1);
    return 1;
}

void signal_sem(int semID, int number) {
    struct sembuf operacje[1];
    operacje[0].sem_num = number;
    operacje[0].sem_op = 1;

    if (semop(semID, operacje, 1) == -1)
        perror("Blad semop (postSemafor): ");
}



int PE(semArray A, semArray B) {
    struct sembuf operacje[2];

    int i = 0;
    for (; i < A.n; i++) {
        operacje[i].sem_num = A.sems[i];
        operacje[i].sem_op = -1;
        operacje[i].sem_flg = SEM_UNDO;
    }
    for (int j = 0; j < B.n; j++) {
        operacje[i].sem_num = B.sems[j];
        operacje[i].sem_op = -1;
        operacje[i].sem_flg = SEM_UNDO;
        i++;
    }

    if (semop(A.semID, operacje, A.n + B.n) == -1) {
        perror("Blad semop (PE):");
        return -1;
    }

    return 1;
};

int VE(semArray A) {
    struct sembuf operacje[2];

    for (int i = 0; i < A.n; i++) {
        operacje[i].sem_num = A.sems[i];
        operacje[i].sem_op = 1;
        operacje[i].sem_flg = SEM_UNDO;
    }

    if (semop(A.semID, operacje, A.n) == -1) {
        perror("Blad semop (VE):");
        return -1;
    }

    return 1;
};