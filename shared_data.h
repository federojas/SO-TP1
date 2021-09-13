#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include "includes.h"

#define SHM_PATH "/shm_buff"
#define SEM_MUTEX "/sem_mutex"
#define SEM_FULL "/sem_full"

typedef struct shared_data_CDT * shared_data_ADT;
shared_data_ADT init_shared_data(char *mutexPath, char *shmPath, int shmSize);
shared_data_ADT open_data(char *mutexPath, char *shmPath, int shmSize);
void close_data(shared_data_ADT data);
void unlink_data(shared_data_ADT data);
int shm_writer(char *buff, char *shmBase);
sem_t *getMutexSem(shared_data_ADT data);
char *getShmBase(shared_data_ADT data);

#endif 