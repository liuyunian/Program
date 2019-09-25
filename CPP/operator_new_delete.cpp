/**
 * 重载全局的operator new delete new[] delete[]
 */

#include <iostream>

void * operator new(size_t size){
    std::cout << "执行了重载的全局operator new" << std::endl;
    return malloc(size);
}

void operator delete(void * p){
    std::cout << "执行了重载的全局operator delete" << std::endl;
    free(p);
}

void * operator new[](size_t size){
    std::cout << "执行了重载的全局operator new[]" << std::endl;
    return malloc(size);
}

void operator delete[](void * p){
    std::cout << "执行了重载的全局operator delete[]" << std::endl;
    free(p);
}

class A{};

int main(){
    int * p = new int();
    delete p;

    char * str = new char[10];
    delete[] str;

    A * p_a = new A();
    delete p_a;

    A * p_as = new A[3];
    delete[] p_as;

    return 0;
}