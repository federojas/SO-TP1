#ifndef LIBUTIL_H
#define LIBUTIL_H

#define SHR_MEM_NAME "/shm_buffer"
#define SEM_NAME "/shr_sem"

int init_shm(char *path, int size, char *shmBase );
        struct meminfoCDT{
    int shmSize, fd;
    char *path, *base;
};            

#endif 