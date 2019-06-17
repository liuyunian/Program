#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

static void 
tLog(char * tip, int num, ...){
    va_list ap;
    char * str = (char *)malloc(sizeof(char) * 1024);
    char * str_print = str;

    strncpy(str, tip, strlen(tip));
    str += strlen(tip);
    strncpy(str, ": ", 2);
    str += 2;

    va_start(ap, num);
    for(int i = 0; i < num; ++ i){
        char * arg = va_arg(ap, char *);
        size_t len = strlen(arg);
        strncpy(str, arg, len);
        str += len;
        if(i < num-1){
            strncpy(str, ", ", 2);
            str += 2;
        }
    }

    printf("%s\n", str_print);
}

int main(){
    tLog("INFO", 3, "liu", "yu", "nian");

    return 0;
}
