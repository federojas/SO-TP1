#include "includes.h"

#include "shared_data.h"

int main(int argc, char *argv[]){
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

    int named_pipe_fd = open(NAMED_PIPE, O_RDONLY);
    if(named_pipe_fd == -1)
        error_handler("open");

    char output[MAX_READ_OUTPUT_SIZE];
    int i=0;
    while(i < task_count) {
        int aux = read(named_pipe_fd, output, MAX_READ_OUTPUT_SIZE);
        if(aux == -1)
            error_handler("read");
        else if(aux != 0) {
            i++;
            printf("%s\n", output);
        }
    }
    if(close(named_pipe_fd) == -1)
        error_handler("close");
    
}
