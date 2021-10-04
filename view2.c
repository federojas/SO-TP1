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

    

    int named_pipe_fd = open(NAMED_PIPE, 0755);
    if(named_pipe_fd == -1)
        error_handler("open");
    char output[MAX_READ_OUTPUT_SIZE];
    int i = 0;
    while(i < 7) {
        if(read(named_pipe_fd, output, MAX_READ_OUTPUT_SIZE) == -1)
            error_handler("read");
        printf("%s", output);
        i++;
    }
    if(close(named_pipe_fd) == -1)
        error_handler("close");
    
}

// 0000 is ---------
// 0666 is rw-rw-rw-
// 0777 is rwxrwxrwx
// 0700 is rwx------
// 0100 is --x------
// 0001 is --------x
// 0002 is -------w-
// 0003 is -------wx
// 0004 is ------r--
// 0005 is ------r-x
// 0006 is ------rw-
// 0007 is ------rwx