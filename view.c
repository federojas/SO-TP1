#define _XOPEN_SOURCE 500

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

int main(int argc, char *argv[]) {
    int task_count = 0;

    if (argc > 2) {
        fprintf(stderr, "Usage: %s <task_count>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (argc == 2) {
        task_count = atoi(argv[1]);
    }
    else if (argc == 1) {
        scanf("%d", &task_count);
    }

    sharedData shared_data = openData(SEM_MUTEX,SHM_PATH, task_count * MAX_READ_OUTPUT_SIZE);

    sem_t *mutexSem=getMutexSem(shared_data);
    char *shmBase=getShmBase(shared_data);   
    int i = 0;
    int offset=0;
    while (i <= task_count) {
        if(sem_wait(mutexSem) == -1) {
            error_handler("sem_wait");
        }
        offset+=printf("%s",shmBase+offset);
        i++;
    }
    
    closeData(shared_data);
    return 0;
}
    
    
    
   

    