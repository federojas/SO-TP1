#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "error_handling.h"

#define MAX_READ_OUTPUT_SIZE 4096
#define STDIN 0
#define STDOUT 1

void solve(char *line);

int main(int argc, char const *argv[])
{
    //desactivamos buffer
    int check = setvbuf(stdout, NULL, _IONBF, 0);
    if(check != 0) {
        perror("setvbuf failed");
    }

    //receive tasks
    char read_output[MAX_READ_OUTPUT_SIZE + 1];
    int read_return = 0;

    while ((read_return = read(STDIN, read_output, MAX_READ_OUTPUT_SIZE)) != 0) {
        if (read_return == -1)
            error_handler("read");

        read_output[read_return] = 0;
        solve(read_output);
    }

    return 0;

}

void solve(char *file){
    char result[MAX_READ_OUTPUT_SIZE + 1]; 
    char minisat_output[MAX_READ_OUTPUT_SIZE + 1];
    
    int check = sprintf(result, "minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\" | tr \"\\n\" \"\\t\" | tr -d \" \\t\"", file);
    if(check == -1) {
        fprintf(stderr, "sprintf error");
    }
    
    FILE* result_stream = popen(result, "r");
    if (result_stream == NULL) {
        error_handler("popen");
    }
    
    int count = fread(minisat_output, sizeof(char), MAX_READ_OUTPUT_SIZE, result_stream);

    minisat_output[count] = 0;

    if (ferror(result_stream)){
        fprintf(stderr, "fread error");
    }

    int number_of_variables, number_of_clauses;
    float cpu_time;
    char satisfiability[14] = {0};

    if (sscanf(minisat_output, "Numberofvariables:%10dNumberofclauses:%10dCPUtime:%10fs%13s", &number_of_variables, &number_of_clauses, &cpu_time, satisfiability) == EOF) {
        error_handler("sscanf");
    }

    //usamos printf para utilizar el pipe en lugar de un write con fd pues es mas comodo
    //nos comunicados por FD 1 con el master
    printf("PID: %d Filename: %s Number of variables: %d Number of clauses: %d CPU time: %fs %s", getpid(), file, number_of_variables, number_of_clauses, cpu_time, satisfiability);
    if(pclose(result_stream) == -1) {
        error_handler("pclose");
    }
}
