#include <stdio.h>
#include <stdlib.h>

int main(){
    int length = 5;
    int * p = NULL;
    p = (int *)malloc(length*sizeof(int));

    for(int i = 0; i<length; i++){
        *(p+i) = i;
    }

    for(int i = length-1; i >= 0; i--){
        printf("%d\n", p[i]);
    }
 
    return 0;
}