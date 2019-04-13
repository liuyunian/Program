#include <stdio.h>
#include <stdlib.h>

int show(int data){
    printf("%d\n", data);
    return data;
}

int main(){
    int (*p)(int) = show;
    int result = (*p)(1);
    printf("%d", result);
    return 0;
}