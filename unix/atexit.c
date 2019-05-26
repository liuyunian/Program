#include <stdio.h>
#include <stdlib.h>

void exit_handler1(){
    printf("INFO: first exit handler\n");
}

void exit_handler2(){
    printf("INFO: second exit handler\n");
}

int main(){
    if(atexit(exit_handler1) != 0){
        printf("ERROR: can't reginster exit_handler1\n");
    }

    if(atexit(exit_handler1) != 0){
        printf("ERROR: can't reginster exit_handler1\n");
    }

    if(atexit(exit_handler2) != 0){
        printf("ERROR: can't reginster exit_handler2\n");
    }

    printf("hello world\n");

    return 0;
}
