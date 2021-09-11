

#include "sharedData.h"

struct sharedDataCDT{
    sem_t *mutexSem, *fullSem;
    int shmSize, shmFd;
    char *shmPath, *mutexPath, *fullPath, *shmBase;
};
sharedData initSharedData(char *mutexPath, char *fullPath, char *shmPath, int shmSize){
    sharedData shared_data= malloc(sizeof(struct sharedDataCDT));
    shared_data->mutexPath=mutexPath;
    shared_data->fullPath=fullPath;
    shared_data->shmPath=shmPath;
    shared_data->shmSize=shmSize;

    //---------------------------SHM OPEN (with creation flag)----------------------------------------------------------------------------------------------------
    //shm open with creation flags 
    shared_data->shmFd=shm_open(shared_data->shmPath, O_CREAT | O_RDWR | O_EXCL, S_IWUSR | S_IRUSR );

    if(shared_data->shmFd<0){
        //error handler
    }
    //puede que aca vaya shmSize+sizeof long o puede que no REVISAR ESTO
    if(ftruncate(shared_data->shmFd,shmSize)<0){
        //error handler
    }

    //--------------------------SEM OPENS (with creation flag)----------------------------------------------------------------------------------------------------
    shared_data->mutexSem = sem_open(mutexPath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
    if(shared_data->mutexSem==SEM_FAILED){
        //error handler
    }
    shared_data->fullSem = sem_open(fullPath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
    if(shared_data->fullSem==SEM_FAILED){
        //error handler
    }
    //-------------------------------------MAPPING----------------------------------------------------------------------------------------------------------------
    shared_data->shmBase =mmap(NULL, shared_data->shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shared_data->shmFd, 0);
    if(shared_data->shmBase == MAP_FAILED){
        //error handler
    } 
    return shared_data;
}

sem_t *getMutexSem(sharedData data){
    return data->mutexSem;
}
sem_t *getFullSem(sharedData data){
    return data->fullSem;
}
char *getShmBase(sharedData data){
    return data->shmBase;
}
