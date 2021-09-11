

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

    //------------------------------------NO SE SI HACE FALTA ESTO
    shm_unlink(shared_data->shmPath);
    sem_unlink(shared_data->fullPath);
    sem_unlink(shared_data->mutexPath);

    //-------------------------------------

    //---------------------------SHM OPEN (with creation flag)----------------------------------------------------------------------------------------------------
    //shm open with creation flags 
    shared_data->shmFd=shm_open(shared_data->shmPath, O_CREAT | O_RDWR | O_EXCL, S_IWUSR | S_IRUSR );

    if(shared_data->shmFd<0){
        //error handler
    }
    //puede que aca vaya shmSize+sizeof long o puede que no REVISAR ESTO
    if(ftruncate(shared_data->shmFd,shmSize+ sizeof(long))<0){
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
    char *shmBase =mmap(NULL, shared_data->shmSize + sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, shared_data->shmFd, 0);
    //(mmap returns a pointer to a block of memory, in this case shmbase is pointing at that block of memory)
    if(shmBase == MAP_FAILED){
        //error handler
    } 
    *(long *)shmBase = 0;

    shared_data->shmBase=shmBase;
    return shared_data;
}
sharedData openData(char *mutexPath, char *fullPath, char *shmPath, int shmSize){
    //basically we do a similar process that in the init but without creation flags
    sharedData shared_data= malloc(sizeof(struct sharedDataCDT));
    shared_data->mutexPath=mutexPath;
    shared_data->fullPath=fullPath;
    shared_data->shmPath=shmPath;
    shared_data->shmSize=shmSize;
    shared_data->mutexSem = sem_open(mutexPath,0, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
    if(shared_data->mutexSem==SEM_FAILED){
        //error handler 
    }
    shared_data->fullSem = sem_open(fullPath, 0, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
    if(shared_data->fullSem==SEM_FAILED){
        //error handler
    }
    //shm open with creation flags 
    shared_data->shmFd=shm_open(shared_data->shmPath, O_RDWR | O_EXCL, S_IWUSR | S_IRUSR );

    if(shared_data->shmFd<0){
        //error handler
    }

    if(ftruncate(shared_data->shmFd,shmSize+ sizeof(long))<0){
        //error handler
    }
    char *shmBase =mmap(NULL, shared_data->shmSize + sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, shared_data->shmFd, 0);
    //(mmap returns a pointer to a block of memory, in this case shmbase is pointing at that block of memory)
    if(shmBase == MAP_FAILED){
        //error handler
    } 

    return shared_data ;

}
void closeData(sharedData data){
    //FALTA TODO EL ERROR HANDLER 
    sem_close(data->fullSem);
    sem_close(data->mutexSem);
    munmap(data->shmBase, data->shmSize+sizeof(long));
    close(data->shmFd);
    free(data);
    
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
