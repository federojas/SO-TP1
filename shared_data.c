// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _XOPEN_SOURCE 500

#include "shared_data.h"

struct shared_data_CDT{
    sem_t *mutexSem;
    int shmSize, shmFd;
    char *shmPath, *mutexPath, *shmBase;
};
int shm_writer(char *buff, char *shmBase){
    int size=strlen(buff);
    shmBase[size]='\n';
    memcpy(shmBase, buff,size);
    return size+1;

}
shared_data_ADT init_shared_data(char *mutexPath, char *shmPath, int shmSize){
    shared_data_ADT shared_data= malloc(sizeof(struct shared_data_CDT));
    shared_data->mutexPath=mutexPath;
    shared_data->shmPath=shmPath;
    shared_data->shmSize=shmSize;
    shm_unlink(shared_data->shmPath);
    sem_unlink(shared_data->mutexPath);

    //--------------------------SEM OPEN (with creation flag)----------------------------------------------------------------------------------------------------
    shared_data->mutexSem = sem_open(mutexPath, O_CREAT |  O_EXCL , 0660, 1 );
    if(shared_data->mutexSem==SEM_FAILED){
        error_handler("sem_open");
    }

    //---------------------------SHM OPEN (with creation flag)----------------------------------------------------------------------------------------------------
    shared_data->shmFd=shm_open(shmPath, O_CREAT | O_RDWR | O_EXCL, S_IWUSR | S_IRUSR );
    if(shared_data->shmFd==-1){
        error_handler("shm_open");
    }
   
    if(ftruncate(shared_data->shmFd,shmSize)==-1){
        error_handler("ftruncate");
    }

    //-------------------------------------MAPPING----------------------------------------------------------------------------------------------------------------
    char *shmBase =mmap(NULL, shmSize, PROT_WRITE, MAP_SHARED, shared_data->shmFd, 0);
    //(mmap returns a pointer to a block of memory, in this case shmbase is pointing at that block of memory)
    if(shmBase == MAP_FAILED){
        error_handler("mmap");
    } 
    shared_data->shmBase=shmBase;
    return shared_data;
}
void unlink_data(shared_data_ADT data){
    if(munmap(data->shmBase, data->shmSize)<0){
        error_handler("munmap");
        return;
    }
    if(shm_unlink(data->shmPath)<0){
        error_handler("shm_unlink");
        return;
    }
    if(sem_unlink(data->mutexPath)<0){
        error_handler("sem_unlink");
        return;
    }
    free(data);
}
shared_data_ADT open_data(char *mutexPath, char *shmPath, int shmSize){

    //similar process init but without creation flags
    shared_data_ADT shared_data = malloc(sizeof(struct shared_data_CDT));
    shared_data->mutexPath=mutexPath;
    shared_data->shmPath=shmPath;
    shared_data->shmSize=shmSize;

    shared_data->mutexSem = sem_open(mutexPath, 0, 0660, 0);
    if(shared_data->mutexSem == SEM_FAILED){
        error_handler("sem_open");
    }

    //shm open with creation flags 
    shared_data->shmFd=shm_open(shared_data->shmPath,  O_RDONLY  , 00400);//not necessary to specify permits

    if(shared_data->shmFd==-1){
        error_handler("shm_open");
    }

    char *shmBase =mmap(NULL, shmSize, PROT_READ, MAP_SHARED, shared_data->shmFd, 0);
    //(mmap returns a pointer to a block of memory, in this case shmbase is pointing at that block of memory)
    if(shmBase == MAP_FAILED){
        error_handler("mmap");
    } 
    shared_data->shmBase=shmBase;
    return shared_data ;

}
void close_data(shared_data_ADT data){
    if(sem_close(data->mutexSem) == -1){
        error_handler("sem_close");
    }
    if(munmap(data->shmBase, data->shmSize) == -1){
        error_handler("munmap");
    }
    if(close(data->shmFd) == -1){
        error_handler("close");
    }
    free(data);  
}
sem_t *get_mutex_sem(shared_data_ADT data){
    return data->mutexSem;
}
char *get_shm_ptr(shared_data_ADT data){
    return data->shmBase;
}
