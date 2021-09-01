#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    FILE * fp = fopen("prueba.txt", "r");
    if(fp == NULL) {
        exit(EXIT_FAILURE);
    }
    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, fp) != -1) {
        fputs(line, stdout);
    }

    
    fclose(fp);
    free(line);

    
    
    return 0;
}
