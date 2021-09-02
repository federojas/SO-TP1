#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    // FILE * fp = fopen("prueba.txt", "r");
    // if(fp == NULL) {
    //     exit(EXIT_FAILURE);
    // }
    // char *line = NULL;
    // size_t len = 0;
    // while(getline(&line, &len, fp) != -1) {
    //     fputs(line, stdout);
    //     printf("\n");
    // }
    
    // fclose(fp);
    // free(line);
    char buff[4];
   int check = read(0, buff, 4);
   if(check == -1)
            printf("Error");
    printf("%s", buff); //se comunica con el parent (fd1 usa printf)
    return 0;
}
