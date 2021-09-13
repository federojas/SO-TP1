// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "includes.h"

void error_handler(char * description) {
    perror(description);
    exit(EXIT_FAILURE);
}