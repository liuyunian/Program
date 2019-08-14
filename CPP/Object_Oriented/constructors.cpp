/**
 * 构造函数
 */
#include <iostream>

class A{
// public:
//     A(){}

public:
    double data;
};

int main(){
    /**
     * new A和new A()的区别
     */
    A * p_a1 = new A;
    std::cout << p_a1->data << std::endl;

    A * p_a2 = new A();
    std::cout << p_a2->data << std::endl;

    return 0;
}