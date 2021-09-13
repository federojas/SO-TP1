#ifndef SHAREDDATA_H
#define SHAREDDATA_H
#define SHM_PATH "/shm_buff"
#define SEM_MUTEX "/sem_mutex"
#define SEM_FULL "/sem_full"
#define SIZE_TEMPORAL_DESPUES_BORRAR 200
#include <semaphore.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "error_handling.h"
#include <stdio.h>
#include <string.h>


typedef struct sharedDataCDT *sharedData;
sharedData initSharedData(char *mutexPath, char *shmPath, int shmSize);
sharedData openData(char *mutexPath, char *shmPath, int shmSize);
void closeData(sharedData data);
void unlinkData(sharedData data);
int shm_writer(char *buff, char *shmBase);
sem_t *getMutexSem(sharedData data);
char *getShmBase(sharedData data);

#endif 