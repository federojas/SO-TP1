#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include "includes.h"

#define SHM_PATH "/shm_buff"
#define SEM_MUTEX "/sem_mutex"
#define NAMED_PIPE "/tmp/named_pipe"

typedef struct shared_data_CDT * shared_data_ADT;
shared_data_ADT init_shared_data(char *mutex_path, char *shm_path, int shmSize);
shared_data_ADT open_data(char *mutex_path, char *shm_path, int shmSize);
void close_data(shared_data_ADT data);
void unlink_data(shared_data_ADT data);
int shm_writer(char *buff, char *shm_ptr);
sem_t *get_mutex_sem(shared_data_ADT data);
char *get_shm_ptr(shared_data_ADT data);

#endif 