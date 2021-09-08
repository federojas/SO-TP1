#include <stdio.h>
#include <stdlib.h>

void error_handler(char * description) {
    perror(description);
    exit(EXIT_FAILURE);
}