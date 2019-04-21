#include <iostream>

class A{
public:
    A(){}
};

int main(){
    auto * p = new int();
    auto * str = new std::string();
    auto * a = new A();

    return 0;
}