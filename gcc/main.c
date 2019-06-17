/* main.c */
#include <stdio.h>
#include "add.h"
#include "sub.h"
 
int main(void){
    int a = 10, b = 12;
    float x = 1.23456, y = 9.87654321;
 
    printf("int a+b IS:%d\n",add_int(a,b));
    printf("int a-b IS:%d\n",sub_int(a,b));
    printf("float a+b IS:%f\n",add_float(x,y));
    printf("float a-b IS:%f\n",sub_float(x,y));
 
    return 0;
}