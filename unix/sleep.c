#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char * argv[]){
    strncpy(argv[0], "test", 4);

    for(int i  = 0; i < argc; ++ i){
        printf("%s\n", argv[i]);
    }

    while(1){
        printf("sleep 1s\n");
        sleep(1);
    }

    return 0;
}