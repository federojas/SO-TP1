#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFF_SIZE 1024
void solve(char *line);

int main(int argc, char const *argv[])
{
    //desactivamos buffer
    int check = setvbuf(stdout, NULL, _IONBF, 0);

    if(check != 0) {
        //ERROR HANDLER 
        printf("Error\n");
    }
    
    for (size_t i = 1; i < argc; i++) {
        solve((char *) argv[i]);
    }
    
    return 0;

}

void solve(char *file){
    char result[BUFF_SIZE + 1]; 
    char minisat_output[BUFF_SIZE + 1];
    
    int check = sprintf(result, "minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"", file);
    if(check == -1) {
        printf("Error");
    }
    
    FILE* result_stream = popen(result, "r");
    if (result_stream == NULL) {
        printf("Error");
    }
    
    int count = fread(minisat_output, sizeof(char), BUFF_SIZE, result_stream);

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
