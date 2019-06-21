#include <stdio.h>
#define NUM 1000

int main(){
#if NUM < 0
    printf("num < 0\n");
#elif NUM >= 0 && NUM < 10 
    printf("0 <= num < 10\n");

#elif NUM >=10 && NUM < 100
    printf("10 <= num < 100\n");

#else 
    printf("num >= 100\n");
#endif

    return 0;
}