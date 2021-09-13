// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include "error_handling.h"

void error_handler(char * description) {
    perror(description);
    exit(EXIT_FAILURE);
}