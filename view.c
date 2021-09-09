#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    if (argc == 1) {
        // ./vista invocado en un pipe (./solve files/* | ./vista)
    } else if (argc == 2) {
        // ./vista invocado con informacion ( ./vista <info> )
    } else {
        fprintf(stderr, "Usage: < %s > or < %s >", "./solve files/* | ./vista", "./vista <info>");
        exit(EXIT_FAILURE);
    }
}