#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

typedef struct semArray {
    int semID;
    int n;
    int *sems;
} semArray;

int get_ftok_key(char key);
int get_msg_id(key_t msg_key, int msgflg);
int get_shm_id(key_t shm_key, size_t size, int shmflg);

int aloc_sem(key_t klucz, int number, int flagi);
void init_sem(int semID, int number, int val);
int free_sem(int semID, int number);
int wait_sem(int semID, int number, int flags);
void signal_sem(int semID, int number);
int PE(semArray A, semArray B);
int VE(semArray A);