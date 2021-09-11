#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include "error_handling.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>
#include "sharedData.h"
#include <semaphore.h>

#define MAX_CHILDS 5
#define MAX_INITIAL_TASKS_PER_CHILD 3
#define INITIAL_TASKS_PER_CHILD(childs_count, pending_tasks) (((MAX_INITIAL_TASKS_PER_CHILD)*childs_count) <= pending_tasks ? MAX_INITIAL_TASKS_PER_CHILD : 1)
#define CALCULATE_CHILDS_COUNT(total_tasks) (MAX_CHILDS > total_tasks ? total_tasks : MAX_CHILDS)
#define MAX_READ_OUTPUT_SIZE 4096
#define STDOUT 1
#define STDIN 0


typedef struct {
    int child_to_parent[2];
    int parent_to_child[2];
    pid_t pid;
} t_child;

void initialize_pipes(t_child pipes[], int childs_count, int * highest_ctp_read_fd);
void close_pipes(t_child child);
void initialize_forks(t_child pipes[], int childs_count, int total_tasks, char * const* tasks, int * current_task_idx);
void build_read_set(t_child pipes[], fd_set * read_set, int childs_count);
void send_task(t_child pipe, char * const* tasks, int * current_task_idx);



const char* slave_file_name = "./slave"; //insert slave file name

int main(int argc, char const *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <files>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int check = setvbuf(stdout, NULL, _IONBF, 0);
    if (check != 0) {
        error_handler("setvbuff");
    }
    
    int total_tasks = argc - 1;
    printf("TOTAL TASKS=%d\n",total_tasks);
    int current_task_idx = 0;
    int solved_tasks = 0;
    char const **tasks = argv + 1;

    // Initialize new childs

    int childs_count = CALCULATE_CHILDS_COUNT(total_tasks);

    t_child pipes[childs_count];

    int highest_ctp_read_fd = -1;
    initialize_pipes(pipes, childs_count, &highest_ctp_read_fd);
    initialize_forks(pipes, childs_count, total_tasks, (char * const*)tasks, &current_task_idx);
    

    //send initial tasks
    int initial_tasks_per_child = INITIAL_TASKS_PER_CHILD(childs_count, total_tasks);

    for(int i = 0; i < childs_count; i++) {
        for(int j = 0; j < initial_tasks_per_child; j++) {
            send_task(pipes[i], (char * const*)tasks, &current_task_idx);
        }
    }

    fd_set read_set, ready_set;

    //initialize all shared data that will be used
    sharedData shared_data=initSharedData(SEM_MUTEX, SEM_FULL,SHM_PATH, total_tasks * MAX_READ_OUTPUT_SIZE );
    sem_t *mutexSem=getMutexSem(shared_data);
    sem_t *fullSem=getMutexSem(shared_data);
    char *shmBase=getShmBase(shared_data);
    //------------------PRUEBAS A VER SI ANDA SHM DESPUES SACAR---------------------------------------
    int i=0;
    while(i<10){
        i++;
        sem_wait(mutexSem);
                        sprintf(shmBase + sizeof(long) + (*(long *)shmBase) * MAX_READ_OUTPUT_SIZE, "%s\n", "hola");
                        //sprintf(shmBase,"%s\n", "hola");                    
                        (*(long *)shmBase)++;

                    sem_post(mutexSem);
                    sem_post(fullSem);
    }
    unlinkData(shared_data);
    while(solved_tasks < total_tasks) {
            //select is destructive
            ready_set=read_set; //OJO ESTO TODAVIA NO HACE NADA
        
        build_read_set(pipes, &read_set, childs_count);

        if (select(highest_ctp_read_fd + 1, &read_set, NULL, NULL, NULL) == -1 ) {
            error_handler("select");
        }
        
        for(int i = 0; i < childs_count; i++) {
            if(FD_ISSET(pipes[i].child_to_parent[0], &read_set)) {
                int read_return;
                char read_output[MAX_READ_OUTPUT_SIZE + 1];
                read_return = read(pipes[i].child_to_parent[0], read_output, MAX_READ_OUTPUT_SIZE);
                if(read_return == -1) {
                    error_handler("read");
                } else if(read_return != 0) {
                    read_output[read_return] = 0;

                    //printf("%s\n", read_output);
                    
                    //assign a signle new task to the current child
                    if (current_task_idx < total_tasks)
                        send_task(pipes[i],(char * const*)tasks, &current_task_idx);
                    solved_tasks++;
                }
            }
        }
    }
    
                              
    //close remaining fd
    for(int i = 0; i < childs_count; i++) {
        if(close(pipes[i].child_to_parent[0]) == -1)
            error_handler("Closing ctpr FD");
        if(close(pipes[i].parent_to_child[1]) == -1)
            error_handler("Closing ptcw FD");
    }
    return 0;
}


void send_task(t_child child_pipes, char * const* tasks, int * current_task_idx) {
    int task_length = strlen(tasks[*current_task_idx]) + 1;
    if (write(child_pipes.parent_to_child[1], tasks[*current_task_idx], task_length)== -1) {
        error_handler("write");
    }
    (*current_task_idx)++;
}

void build_read_set(t_child pipes[], fd_set * read_set, int childs_count) {
    FD_ZERO(read_set); //set reinitialized
    for (int i = 0; i < childs_count; i++) {
            FD_SET(pipes[i].child_to_parent[0], read_set);
    }
}

void initialize_pipes(t_child pipes[], int childs_count, int * highest_ctp_read_fd) { 
    for (int i = 0; i < childs_count; i++) {
            if (pipe(pipes[i].child_to_parent) == -1) {
                error_handler("Pipe ctp");
            }
            if(pipe(pipes[i].parent_to_child) == -1) {
                error_handler("Pipe ptc");
            }
            if(pipes[i].child_to_parent[0] > *highest_ctp_read_fd) {
                *highest_ctp_read_fd = pipes[i].child_to_parent[0];
            }
    }

    return;
}

void close_pipes(t_child child){
    if(close(child.parent_to_child[0]) == -1)
        error_handler("Closing ptcr FD");
    if(close(child.parent_to_child[1]) == -1)
        error_handler("Closing ptcw FD");
    if(close(child.child_to_parent[0]) == -1)
        error_handler("Closing ctpr FD");
    if(close(child.child_to_parent[1]) == -1)
        error_handler("Closing ctpw FD");
}


void initialize_forks(t_child pipes[], int childs_count, int total_tasks, char * const* tasks, int * current_task_idx) {
    
    char *const *execv_parameter = NULL;
    
    for(int i = 0; i < childs_count; i++) {
       
        if ((pipes[i].pid = fork()) == -1) {
            error_handler("fork");
        }
        //child
        else if(pipes[i].pid == 0) {
            if(dup2(pipes[i].child_to_parent[1], STDOUT)  == -1)
                error_handler("dup2: ");
            if(dup2(pipes[i].parent_to_child[0], STDIN) == -1)
                error_handler("dup2: ");
            close_pipes(pipes[i]);
            if (execv(slave_file_name, execv_parameter) == -1) {
                perror("execv");  
            }
        } 
        //parent
        else {
            //close unused fd
            if(close(pipes[i].child_to_parent[1]) == -1)
               error_handler("Closing ctpw FD");
            if(close(pipes[i].parent_to_child[0]) == -1)
               error_handler("Closing ptcr FD");
        }
    }

}

