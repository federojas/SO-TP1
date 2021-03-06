// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "includes.h"
#include "shared_data.h"

int main(int argc, char *argv[]) {
    int task_count = 0;

    if (argc > 2) {
        fprintf(stderr, "Usage: %s <task_count>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else if (argc == 2) {
        task_count = atoi(argv[1]);
    }
    else if (argc == 1) {
        scanf("%d", &task_count);
    }

    shared_data_ADT shared_data = open_data(SEM_MUTEX,SHM_PATH, task_count * MAX_READ_OUTPUT_SIZE);

    sem_t *mutex_sem = get_mutex_sem(shared_data);
    char *shm_ptr = get_shm_ptr(shared_data);   
    int i = 0;
    int offset=0;
    while (i <= task_count) {
        if(sem_wait(mutex_sem) == -1) {
            error_handler("sem_wait");
        }
        offset += printf("%s", shm_ptr + offset);
        i++;
    }
    
    unlink_data(shared_data);
    return 0;
}
    
    
    
   

    