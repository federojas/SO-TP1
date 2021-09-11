#ifndef SHAREDDATA_H
#define SHAREDDATA_H
#define SHM_PATH "/shm_buff"
#define SEM_MUTEX "/sem_mutex"
#define SEM_FULL "/sem_full"

#include <semaphore.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "error_handling.h"


typedef struct sharedDataCDT *sharedData;
sharedData initSharedData(char *mutexPath, char *fullPath, char *shmPath, int shmSize);
sem_t *getMutexSem();
sem_t *getFullSem();
char *getShmBase();
#endif 