/**
 * 万能指针void *
*/

#include <stdio.h>

void testFunc(void * arg){
    printf("testFunc\n");
}

int main(){
    int a = 5;
    double b = 5.2;
    char c = 'a';


    testFunc(&a);
    testFunc(&a);
    testFunc(&a);

    return 0;
}