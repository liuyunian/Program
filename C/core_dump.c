#include <stdio.h>

int main(){
    int *null_ptr = NULL;
    *null_ptr = 10;            //对空指针指向的内存区域写,会发生段错误
    
    return 0;
}