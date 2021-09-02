
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
    // if(argc < 2) {
    //     printf("Error: Invalid amount of arguments.\n");
    //     exit(EXIT_FAILURE);
    // }
    char* const* execArg = NULL;
    int parent_to_child[2];
    int child_to_parent[2];
    pipe(parent_to_child);
    pipe(child_to_parent);
    int check = fork();

    //child
    if(check == 0) {
        dup2(parent_to_child[0], 0);
        dup2(child_to_parent[1], 1);

        close(parent_to_child[0]);
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        close(child_to_parent[1]);

        execv("./slave", execArg);
    }
    //parent
    else {
        check =  write(parent_to_child[1], "HOLA", 4); 
        if(check == -1)
            printf("Error");
    
        char buff[4];
        check = read(child_to_parent[0], buff, 4);
        if(check == -1)
            printf("Error");
        printf("%s\n", buff); //imprime (stdout)
    }
   
    return 0;
}

