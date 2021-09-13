#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include "includes.h"

#define SHM_PATH "/shm_buff"
#define SEM_MUTEX "/sem_mutex"
#define SEM_FULL "/sem_full"

typedef struct shared_data_CDT * shared_data;
shared_data init_shared_data(char *mutexPath, char *shmPath, int shmSize);
shared_data open_data(char *mutexPath, char *shmPath, int shmSize);
void close_data(shared_data data);
void unlink_data(shared_data data);
int shm_writer(char *buff, char *shmBase);
sem_t *getMutexSem(shared_data data);
char *getShmBase(shared_data data);

#endif 