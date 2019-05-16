#include <stdio.h>
extern int add(int, int);

int main(){
    int i = 1, j = 2;
    int sum = add(i, j);
    printf("sum = %d\n", sum);

    return 0;
}