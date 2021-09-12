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
    shared_data->shmFd=shm_open(shared_data->shmPath, O_CREAT | O_RDWR ,00660 );
    if(shared_data->shmFd==-1){
        error_handler("shm_open");
    }
    //puede que aca vaya shmSize+sizeof long o puede que no REVISAR ESTO
    //if(ftruncate(shared_data->shmFd,shmSize+ sizeof(long))==-1){
    if(ftruncate(shared_data->shmFd,SIZE_TEMPORAL_DESPUES_BORRAR)==-1){
        error_handler("ftruncate");
    }

    //--------------------------SEM OPENS (with creation flag)----------------------------------------------------------------------------------------------------
    shared_data->mutexSem = sem_open(mutexPath, O_CREAT |  O_EXCL , 0660, 1 );
    if(shared_data->mutexSem==SEM_FAILED){
        error_handler("sem_open");
    }
    shared_data->fullSem = sem_open(fullPath, O_CREAT | O_EXCL,  0660, 0);
    if(shared_data->fullSem==SEM_FAILED){
        error_handler("sem_open");
    }
    //-------------------------------------MAPPING----------------------------------------------------------------------------------------------------------------
    char *shmBase =mmap(NULL, SIZE_TEMPORAL_DESPUES_BORRAR, PROT_READ | PROT_WRITE, MAP_SHARED, shared_data->shmFd, 0);
    //(mmap returns a pointer to a block of memory, in this case shmbase is pointing at that block of memory)
    if(shmBase == MAP_FAILED){
        error_handler("mmap");
    } 
    *(long *)shmBase = 0;

    shared_data->shmBase=shmBase;
    return shared_data;
}
void unlinkData(sharedData data){
    if(munmap(data->shmBase, data->shmSize+ sizeof(long))<0){
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
    if(sem_unlink(data->fullPath)<0){
        error_handler("sem_unlink");
        return;
    }
    free(data);
    
}
sharedData openData(char *mutexPath, char *fullPath, char *shmPath, int shmSize){

    //basically we do a similar process that in the init but without creation flags
    sharedData shared_data= malloc(sizeof(struct sharedDataCDT));
    shared_data->mutexPath=mutexPath;
    shared_data->fullPath=fullPath;
    shared_data->shmPath=shmPath;
    shared_data->shmSize=shmSize;
    shared_data->mutexSem = sem_open(mutexPath, 0, 0660, 1);
    if(shared_data->mutexSem == SEM_FAILED){
        error_handler("sem_open");
    }
    shared_data->fullSem = sem_open(fullPath, 0, 0660, 0);
    if(shared_data->fullSem == SEM_FAILED){
        error_handler("sem_open");
    }
    //shm open with creation flags 
    shared_data->shmFd=shm_open(shared_data->shmPath, O_RDWR | O_EXCL, 1);//not necessary to specify permits

    if(shared_data->shmFd==-1){
        error_handler("shm_open");
    }

    if(ftruncate(shared_data->shmFd,SIZE_TEMPORAL_DESPUES_BORRAR)==-1){
        error_handler("ftruncate");
    }
    char *shmBase =mmap(NULL, SIZE_TEMPORAL_DESPUES_BORRAR, PROT_READ | PROT_WRITE, MAP_SHARED, shared_data->shmFd, 0);
    //(mmap returns a pointer to a block of memory, in this case shmbase is pointing at that block of memory)
    if(shmBase == MAP_FAILED){
        error_handler("mmap");
    } 

    return shared_data ;

}
void closeData(sharedData data){
    if(sem_close(data->fullSem) == -1){
        error_handler("sem_close");
    }
    if(sem_close(data->mutexSem) == -1){
        error_handler("sem_close");
    }
    if(munmap(data->shmBase, data->shmSize+sizeof(long)) == -1){
        error_handler("munmap");
    }
    if(close(data->shmFd) == -1){
        error_handler("close");
    }
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
