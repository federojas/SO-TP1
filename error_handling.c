#include <stdio.h>
#include <stdlib.h>
#include "error_handling.h"

void error_handler(char * description) {
    perror(description);
    exit(EXIT_FAILURE);
}