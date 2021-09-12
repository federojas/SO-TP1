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

int main(int argc, char *argv[]) {
    sleep(3);
    int task_count = 0;
    if (argc > 2) {
        fprintf(stderr, "Usage: %s <task_count>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (argc == 2) {
        task_count = atoi(argv[1]);
    }
    else if (argc == 1) {
        scanf("%10d", &task_count);
    }
    sharedData shared_data=openData(SEM_MUTEX, SEM_FULL,SHM_PATH, task_count * MAX_READ_OUTPUT_SIZE);

    sem_t *mutexSem=getMutexSem(shared_data);
    sem_t *fullSem=getMutexSem(shared_data);
    char *shmBase=getShmBase(shared_data);   
    
    int i = 0;
    
    while (i < task_count)
    {
        sem_wait(fullSem);
        sem_wait(mutexSem);
        (*(long *)shmBase)--;
        printf("%s", shmBase + sizeof(long) + (*(long *)shmBase) * MAX_READ_OUTPUT_SIZE);
        sem_post(mutexSem);
        i++;
    }
    
    closeData(shared_data);
    return 0;
}
    
    
    
   

    