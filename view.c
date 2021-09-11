#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define SHM_NAME "/shm"

int main(int argc, char **argv) {
    int total_tasks;

    if (argc == 1) {
        // ./vista invocado en un pipe (./solve files/* | ./vista)
    } else if (argc == 2) {
        // ./vista invocado con informacion ( ./vista <info> )
    } else {
        fprintf(stderr, "Usage: < %s > or < %s >", "./solve files/* | ./vista", "./vista <info>");
        exit(EXIT_FAILURE);
    }

    sharedDataADT data = openSharedData();
    sem_t *full_sem = get_full(data);
    sem_t *mutex_sem = get_mutex(data);
    char *shm_base = get_shm_base(data);

    int i = 0;
    while (i < total_tasks) {
        sem_wait(full_sem);
        sem_wait(mutex_sem);
    }
}