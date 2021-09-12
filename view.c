#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "sharedData.h"
#include <stdio.h>
#define MAX_READ_OUTPUT_SIZE 4096

#define TOTAL_TASKS_MAGIK_NUMBER 22 //SACAR ESTO DESPUES ES SOLO PARA PROBAR 

int main(int argc, char **argv) {

    int i=0;
    sharedData shared_data=openData(SEM_MUTEX, SEM_FULL,SHM_PATH, SIZE_TEMPORAL_DESPUES_BORRAR );
    sem_t *mutexSem=getMutexSem(shared_data);
    sem_t *fullSem=getMutexSem(shared_data);
    char *shmBase=getShmBase(shared_data);    
    printf("%s\n",shmBase);
    //char *aux=malloc(100);
    sleep(1);
    
    //printf("%s\n",shmBase);

    while(i<10){
        i++;
        
        sem_wait(mutexSem);
        sem_wait(fullSem);
       // memcpy(aux, shmBase, sizeof("chau ")*10);
        printf("la concha de tu madre ");
        // (*(long *)shmBase)--;
        // printf("%s", shmBase + sizeof(long) + (*(long *)shmBase) * MAX_READ_OUTPUT_SIZE);
        // printf("asdasdasdasda\n");
        sem_post(mutexSem);

    }
    closeData(shared_data);
}