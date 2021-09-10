#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

#include "libUtil.h"

// struct meminfoCDT{
//     int shmSize, fd;
//     char *path, *base;
// };
int init_shm(char *path, int size, char *shmBase ){
    int fd=shm_open(path,O_CREAT | O_RDWR | O_EXCL,S_IRUSR | S_IXUSR );
    if (fd == -1){
        //"error manager"
    }
    if(ftruncate(fd, size)==-1){
        //error manager
    }
    shmBase = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (&shmBase == MAP_FAILED)
        //"error manager"

    return fd;

} 