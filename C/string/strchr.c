#include <stdio.h>
#include <string.h>

int main(){
    char str[] = "listenPort = 80";
    char * pos = strchr(str, '=');

    printf("%s\n", pos);

    return 0;
}