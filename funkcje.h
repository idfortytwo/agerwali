#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

int get_ftok_key(char key);
int get_shm_id(key_t shm_key, size_t size, int shmflg);

int aloc_sem(key_t klucz, int number, int flagi);
void init_sem(int semID, int number, int val);
int free_sem(int semID, int number);

int PE(int semID, int* A, int lenA, int* B, int lenB);
int VE(int semID, int* A, int lenA);