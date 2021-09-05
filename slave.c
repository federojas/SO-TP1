#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFF_SIZE 1024
void solve(char *line);

int main(int argc, char const *argv[])
{
    /*
    //desactivamos buffer
    int check = setvbuf(stdout, NULL, _IONBF, 0);
    if(check != 0) {
        //ERROR HANDLER 
        printf("Error\n");
    }

    // FILE * fp = fopen("prueba.txt", "r");
    // if(fp == NULL) {
    //     exit(EXIT_FAILURE);
    // }
    
    //https://www.youtube.com/watch?v=TKKQERrrt5o
    char *line = NULL;
    size_t len = 0;
    while(getline(&line, &len, 0) != -1) {
        solve(line);
        // fputs(line, stdout);
        // printf("\n");
    }
    //CHECKEAR ERRNO DEL GETLINE Y MANEJAR ERROR -1
    
    // fclose(fp);
    free(line);
    */

    
//     char buff[4];
//    int check = read(0, buff, 4);
//    if(check == -1)
//         printf("Error"); //aca no se imprime (pipe)
//     printf("%s", buff); //se comunica con el parent (fd1 usa printf)

    solve("hanoi4.cnf");
    return 0;
}

void solve(char *file){
    char command[BUFF_SIZE];   
    sprintf(command, "minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\" | tr \"\\n\" \"\t\" | tr -d \" \t\"", file);
    system(command);
    printf("\n");
}
