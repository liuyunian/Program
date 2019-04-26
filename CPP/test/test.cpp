#include <iostream>

template <typename F, typename T>
void testFunc(F func, T && data){
    func(std::forward<T>(data));
}

void print(int data){
    std::cout << data << std::endl;
}

void print2(int & data){
    ++ data;
    std::cout << data << std::endl;
}

void print3(int && data){
    std::cout << data << std::endl;
}

int main(){
    testFunc(print, 10);

    int a = 8;
    testFunc(print2, a);
    std::cout << a << std::endl;

    testFunc(print3, 10);
    testFunc(print3, std::move(a));
    return 0;
}