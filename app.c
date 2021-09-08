#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAX_INIT_CHILDS 5
#define MIN_TASKS_PER_CHILD 3
#define TASKS_PER_CHILD(childs_count, pending_tasks) (((MIN_TASKS_PER_CHILD)*childs_count) > pending_tasks ? 1 : MIN_TASKS_PER_CHILD)
#define MAX_READ_OUTPUT_SIZE 4096


typedef struct {
    int child_to_parent[2];
    int parent_to_child[2];
    pid_t pid;
    size_t open;
} t_child;

void initialize_pipes(t_child pipes[], size_t childs_count, size_t * max_fd);
void close_pipes(t_child child);
void initialize_forks(t_child pipes[], size_t childs_count, size_t total_tasks, char * const* tasks, size_t * task_idx);
void build_read_set(t_child pipes[], fd_set * read_set, size_t childs_count);


const char* slave_file_name = "./slave"; //insert slave file name

int main(int argc, char const *argv[])
{
    if(argc < 2) {
        printf("Error: Invalid amount of arguments.\n");
        exit(EXIT_FAILURE);
    }

    // Sets a buffer with size zero, i.e, disables buffering on stdout
    // _IONBF means No buffer is used. Each I/O operation is written ASAP
    int check = setvbuf(stdout, NULL, _IONBF, 0);
    if (check != 0) {
        //ERROR HANDLER 
        printf("Error: setvbuff\n");
    }
    
    size_t total_tasks = argc - 1;
    size_t task_idx = 0;
    size_t solved_tasks = 0;
    char const **tasks = argv + 1;

    // Initialize new childs

    size_t childs_count = MAX_INIT_CHILDS > total_tasks ? total_tasks : MAX_INIT_CHILDS;

    t_child pipes[childs_count];

    size_t max_fd = -1;
    initialize_pipes(pipes, childs_count, &max_fd);
    initialize_forks(pipes, childs_count, total_tasks, (char * const*)tasks, &task_idx);
    
    while(solved_tasks < total_tasks) {
        fd_set read_set;
        build_read_set(pipes, &read_set, childs_count);
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        if (select(max_fd + 1, &read_set, NULL, NULL, &timeout) == -1 ) {
            printf("error");
        }

        for(int i = 0; i < childs_count; i++) {
            if(FD_ISSET(pipes[i].child_to_parent[0], &read_set)) {
                size_t read_return;
                char read_output[MAX_READ_OUTPUT_SIZE];
                read_return = read(pipes[i].child_to_parent[0], read_output, MAX_READ_OUTPUT_SIZE);
                if(read_return == -1) {
                    printf("error");
                } else if(read_return == 0) {
                    pipes[i].open = 0;
                    close(pipes[i].child_to_parent[0]);
                } else {
                    printf("%s", read_output);
                }
            }
        }

    }
    

    return 0;
}

void build_read_set(t_child pipes[], fd_set * read_set, size_t childs_count) {
    FD_ZERO(read_set); //set reinitialized
    for (int i = 0; i < childs_count; i++) {
        if (pipes[i].open == 1) {
            FD_SET(pipes[i].child_to_parent[0], read_set);
        }
    }
}

void initialize_pipes(t_child pipes[], size_t childs_count, size_t * max_fd) {
    for (int i = 0; i < childs_count; i++) {
            if (pipe(pipes[i].child_to_parent) == -1) {
               printf("error");
            }
            if(pipe(pipes[i].parent_to_child) == -1) {
                printf("error");
            }
            if(pipes[i].parent_to_child[1] > *max_fd) {
                *max_fd = pipes[i].parent_to_child[1];
            }
            pipes[i].open = 1;
    }
    return;
}

void close_pipes(t_child child){
    if(close(child.parent_to_child[0]) == -1)
        printf("error");
    if(close(child.parent_to_child[1]) == -1)
        printf("error");
    if(close(child.child_to_parent[0]) == -1)
        printf("error");
    if(close(child.child_to_parent[1]) == -1)
        printf("error");
}

void initialize_forks(t_child pipes[], size_t childs_count, size_t total_tasks, char * const* tasks, size_t * task_idx) {
    
    int tasks_per_child = TASKS_PER_CHILD(childs_count, total_tasks);
    for(int i = 0; i < childs_count; i++) {
        char * child_tasks[tasks_per_child + 1];
        child_tasks[tasks_per_child] = NULL;
        if ((pipes[i].pid = fork()) == -1) {
            printf("error");
        }
        //child
        else if(pipes[i].pid == 0) {
            if(dup2(pipes[i].child_to_parent[1], 1)  == -1)
                printf("error");
            if(dup2(pipes[i].parent_to_child[0], 0) == -1)
                printf("error");
            close_pipes(pipes[i]);
            for(int j = 0; j < tasks_per_child; j++)
                child_tasks[j] = tasks[(*task_idx)++];
            execv(slave_file_name, child_tasks);
            printf("fallo"); //si llego hasta aca el programa es porque retorno el execv -> error
        } 
        //parent
        else {
            *task_idx += tasks_per_child;
            if(close(pipes[i].child_to_parent[1]) == -1)
                printf("error");
            if(close(pipes[i].parent_to_child[0]) == -1)
                printf("error");
        }
    }

}

