#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "sharedData.h"
#define MAX_READ_OUTPUT_SIZE 4096

#define TOTAL_TASKS_MAGIK_NUMBER 21 //SACAR ESTO DESPUES ES SOLO PARA PROBAR 

int main(int argc, char **argv) {

    int i=0;
    sharedData shared_data=openData(SEM_MUTEX, SEM_FULL,SHM_PATH, TOTAL_TASKS_MAGIK_NUMBER * MAX_READ_OUTPUT_SIZE );
    sem_t *mutexSem=getMutexSem(shared_data);
    sem_t *fullSem=getMutexSem(shared_data);
    char *shmBase=getShmBase(shared_data);    
    while(i<10){
        i++;
        sleep(1);
        sem_wait(fullSem);
        sem_wait(mutexSem);
        (*(long *)shmBase)--;
        printf("%s", shmBase + sizeof(long) + (*(long *)shmBase) * MAX_READ_OUTPUT_SIZE);
        sem_post(mutexSem);

    }
    closeData(shared_data);
}