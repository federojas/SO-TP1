// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
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
    // char read_output[MAX_READ_OUTPUT_SIZE + 1];
    // int read_return = 0;

    // while ((read_return = read(STDIN, read_output, MAX_READ_OUTPUT_SIZE)) != 0) {
    //     if (read_return == -1)
    //         error_handler("read");
    //     read_output[read_return] = 0;
    //     solve(read_output);
    // }

    char *file = NULL;
    size_t len = 0;

    while ((getline(&file, &len, stdin)) > 0) {
        file[strcspn(file, "\n")] = 0;
        solve(file);
    }

    free(file);

    return 0;

}

void solve(char *file){
    char result[MAX_READ_OUTPUT_SIZE]; 
    char minisat_output[MAX_READ_OUTPUT_SIZE];
    
    int check = sprintf(result, "minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\" | tr \"\\n\" \"\\t\" | tr -d \" \\t\"", file);
    if(check == -1) {
        error_handler("sprintf");
    }
    
    FILE* result_stream = popen(result, "r");
    if (result_stream == NULL) {
        error_handler("popen");
    }
    
   if(fgets(minisat_output, MAX_READ_OUTPUT_SIZE, result_stream) == NULL) {
       error_handler("fgets");
   }

    if (ferror(result_stream)){
        error_handler("sprintf");
    }

    int number_of_variables, number_of_clauses;
    float cpu_time;
    char satisfiability[14] = {0};

    if(pclose(result_stream) == -1) {
        error_handler("pclose");
    }

    if(sscanf(minisat_output, "Numberofvariables:%10dNumberofclauses:%10dCPUtime:%10fs%13s", &number_of_variables, &number_of_clauses, &cpu_time, satisfiability) == EOF){
        error_handler("sscanf");
    }

    //usamos printf para utilizar el pipe en lugar de un write con fd pues es mas comodo
    //nos comunicados por FD 1 con el master
    printf("PID: %d Filename: %s Number of variables: %d Number of clauses: %d CPU time: %fs %s", getpid(), file, number_of_variables, number_of_clauses, cpu_time, satisfiability);
    
}
