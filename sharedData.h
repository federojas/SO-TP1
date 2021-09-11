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
sharedData openData(char *mutexPath, char *fullPath, char *shmPath, int shmSize);
void closeData(sharedData data);
sem_t *getMutexSem(sharedData data);
sem_t *getFullSem(sharedData data);
char *getShmBase(sharedData data);
#endif 