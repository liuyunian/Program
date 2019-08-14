/**
 * 定位new语法
 */

#include <iostream>

class A{
public:
    A(){
        std::cout << "A类中的构造函数被调用" << std::endl;
    }

    ~A(){
        std::cout << "A类中的析构函数被调用" << std::endl;
    }
};

int main(){
    size_t size = sizeof(A);
    void * p1 = malloc(size);
    A * p_a = new (p1) A();

    // delete p_a;
    p_a->~A();
    free(p_a);

    return 0;
}