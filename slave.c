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
    char result[BUFF_SIZE]; 
    char minisat_output[4096];
    
    int check = sprintf(result, "minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"", file);
    if(check == -1) {
        printf("Error");
    }
    
    FILE* result_stream = popen(result, "r");
    if (result_stream == NULL) {
        printf("Error");
    }
    
    int count = fread(minisat_output, sizeof(char), 4096, result_stream);

    minisat_output[count] = 0;

    if (ferror(result_stream)){
        printf("Error");
    }

    //usamos printf para utilizar el pipe en lugar de un write con fd pues es mas comodo
    //nos comunicados por FD 1 con el master
    printf("%s %s %d", minisat_output, file, getpid());

    if(pclose(result_stream) == -1) {
        printf("Error");
    }

}
