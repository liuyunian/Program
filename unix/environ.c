#include <stdio.h>
#include <stdlib.h>

extern char ** environ;

int main(){
    char ** env = environ;
    while(*env){
        printf("%s\n", *env);
        ++ env;
    }

    char * path = getenv("PATH");
    puts(path);

    return 0;
}