#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <semaphore.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "error_handling.h"

#define MAX_READ_OUTPUT_SIZE 4096
#define STDIN 0
#define STDOUT 1

#endif 