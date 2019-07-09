#include <stdio.h>
#include <stdlib.h>

extern char ** environ;

int main(int argc, char * argv[]){
    for(int i = 0; i < argc; ++ i){
        printf("argv[%d] = %s, %p\n", i, argv[i], argv[i]);
    }

    printf("environ[0] = %s, %p\n", environ[0], environ[0]);

    return 0;
}