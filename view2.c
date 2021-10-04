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

    if(mkfifo(NAMED_PIPE, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1) {
        error_handler("named pipe");
    }

    int named_pipe_fd = open(NAMED_PIPE, 0777);
    char output[MAX_READ_OUTPUT_SIZE];
    int i = 0;
    while(i < 7) {
        read(named_pipe_fd, output, MAX_READ_OUTPUT_SIZE);
        printf("%s", output);
        i++;
    }
    close(named_pipe_fd);
    unlink(NAMED_PIPE);
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