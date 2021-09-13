// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "includes.h"
#include "shared_data.h"

#define MAX_CHILDS 5
#define MAX_INITIAL_TASKS_PER_CHILD 3
#define INITIAL_TASKS_PER_CHILD(childs_count, pending_tasks) (((MAX_INITIAL_TASKS_PER_CHILD)*(childs_count)) <= (pending_tasks) ? MAX_INITIAL_TASKS_PER_CHILD : 1)
#define CALCULATE_CHILDS_COUNT(total_tasks) (MAX_CHILDS > (total_tasks) ? (total_tasks) : MAX_CHILDS)

typedef struct {
    int child_to_parent[2];
    int parent_to_child[2];
    pid_t pid;
} t_child;

void initialize_pipes(t_child pipes[], int childs_count, int * highest_ctp_read_fd);
void close_pipes(t_child * childs, int childs_count);
void initialize_forks(t_child pipes[], int childs_count, int total_tasks, char * const* tasks, int * current_task_idx);
void build_read_set(t_child pipes[], fd_set * read_set, int childs_count);
void send_task(t_child pipe, char * const* tasks, int * current_task_idx);
void initialize_shared_memory(shared_data_ADT * shared_data, sem_t ** mutex_sem, char ** shm_ptr, int total_tasks);
void send_initial_tasks(t_child * pipes, char * const* tasks, int childs_count, int initial_tasks_per_child, int * current_task_idx);
void free_childs(t_child * pipes, int childs_count);

const char* slave_file_name = "./slave"; //insert slave file name
const char* answers_file_name = "answers.txt"; //insert answers file name


int main(int argc, char const *argv[]) {
    //comentario de prueba
    
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <files>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int check = setvbuf(stdout, NULL, _IONBF, 0);
    if (check != 0) {
        error_handler("setvbuff");
    }
    
    int total_tasks = argc - 1;
    int current_task_idx = 0;
    int solved_tasks = 0;
    char const **tasks = argv + 1;

    FILE * solve_file;
    solve_file = fopen(answers_file_name, "w");
    if (solve_file == NULL) {
        error_handler("fopen");
    }

    shared_data_ADT shared_data = NULL;
    char * shm_ptr = NULL;
    sem_t * mutex_sem = NULL;

    initialize_shared_memory(&shared_data, &mutex_sem, &shm_ptr, total_tasks);
    
    sleep(2);
    printf("%d", total_tasks);

    int childs_count = CALCULATE_CHILDS_COUNT(total_tasks);
    t_child pipes[childs_count];

    int highest_ctp_read_fd = -1;
    initialize_pipes(pipes, childs_count, &highest_ctp_read_fd);
    initialize_forks(pipes, childs_count, total_tasks, (char * const*)tasks, &current_task_idx);
    
    int initial_tasks_per_child = INITIAL_TASKS_PER_CHILD(childs_count, total_tasks);
    send_initial_tasks(pipes, (char * const*)tasks, childs_count, initial_tasks_per_child, &current_task_idx);

    int offset=0;
    while(solved_tasks < total_tasks) {
        fd_set read_set;
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

                    char * answer = strtok(read_output, "\n");
                    while(answer != NULL) {

                        offset += shm_writer(answer, shm_ptr + offset);
                        if(sem_post(mutex_sem) == -1) {
                            error_handler("sem_post");
                        }


                        if(fprintf(solve_file, "%s\n", answer)<0){
                            error_handler("fprintf");
                        }
                        
                        answer = strtok(NULL, "\n");
        
                        solved_tasks++;
                    }
                        //assign a single new task to the current child
                        if(current_task_idx < total_tasks){
                            send_task(pipes[i],(char * const*)tasks, &current_task_idx);
                            if (write(pipes[i].parent_to_child[1], "\n", 1) == -1)
                                error_handler("write");
                        }
                }
            }
        }
    }
    if(fclose(solve_file) == EOF)
        error_handler("fclose");
    free_childs(pipes, childs_count);
    close_data(shared_data);
    return 0;
}


void send_task(t_child child_pipes, char * const* tasks, int * current_task_idx) {
    int task_length = strlen(tasks[*current_task_idx]);
    if (write(child_pipes.parent_to_child[1], tasks[*current_task_idx], task_length) == -1) {
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

void initialize_pipes(t_child * pipes, int childs_count, int * highest_ctp_read_fd) { 
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

void close_pipes(t_child * childs, int childs_count){
    for(int i = 0; i < childs_count; i++) {
        close(childs[i].parent_to_child[0]);
        close(childs[i].parent_to_child[1]);
        close(childs[i].child_to_parent[0]);
        close(childs[i].child_to_parent[1]);
    } 
}


void initialize_forks(t_child * pipes, int childs_count, int total_tasks, char * const* tasks, int * current_task_idx) {
    
    char *const *execv_parameter = NULL;
    
    for(int i = 0; i < childs_count; i++) {
       
        if ((pipes[i].pid = fork()) == -1) {
            error_handler("fork");
        }
        //child
        else if(pipes[i].pid == 0) {
            if(dup2(pipes[i].child_to_parent[1], STDOUT)  == -1)
                error_handler("dup2 ctpw");
            if(dup2(pipes[i].parent_to_child[0], STDIN) == -1)
                error_handler("dup2 ptcr");
            close_pipes(pipes, childs_count);
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

void initialize_shared_memory(shared_data_ADT * shared_data, sem_t ** mutex_sem, char ** shm_ptr, int total_tasks) {
    *shared_data = init_shared_data(SEM_MUTEX, SHM_PATH, total_tasks * MAX_READ_OUTPUT_SIZE);
    *mutex_sem=get_mutex_sem(*shared_data);
    *shm_ptr=get_shm_ptr(*shared_data);
}
    
void send_initial_tasks(t_child * pipes, char * const* tasks, int childs_count, int initial_tasks_per_child, int * current_task_idx) {
    for(int i = 0; i < childs_count; i++) {
        for(int j = 0; j < initial_tasks_per_child; j++) {
            send_task(pipes[i], (char * const*)tasks, current_task_idx);
            if (write(pipes[i].parent_to_child[1], "\n", 1) == -1)
                error_handler("write");
        }
    }
}

void free_childs(t_child * pipes, int childs_count) {
    for(int i = 0; i < childs_count; i++) {
        if(close(pipes[i].child_to_parent[0]) == -1)
            error_handler("Closing ctpr FD");
        if(close(pipes[i].parent_to_child[1]) == -1)
            error_handler("Closing ptcw FD");
        if(waitpid(pipes[i].pid, NULL, 0) == -1)
            error_handler("wait");
    }  
}
