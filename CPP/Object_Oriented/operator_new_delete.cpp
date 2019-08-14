/**
 * 重载类中的new和delete操作符
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

    static void * operator new(size_t size){
        std::cout << "A类中的重载new操作符函数被调用" << std::endl;
        // ... 其他代码
        A * p_a = static_cast<A *>(malloc(size));
        return p_a;
    }

    void operator delete(void * p){
        std::cout << "A类中的重载delete操作符函数被调用" << std::endl;
        // ... 其他代码
        free(p);
    }

    static void * operator new[](size_t size){
        std::cout << "A类中的重载new[]操作符函数被调用" << std::endl;
        // ... 其他代码
        A * p_a = static_cast<A *>(malloc(size));
        return p_a;
    }

    void operator delete[](void * p){
        std::cout << "A类中的重载delete[]操作符函数被调用" << std::endl;
        // ... 其他代码
        free(p);
    }
};

int main(){
    // A * a = new A();
    // delete a;

    A * a = new A[3]();
    delete[] a;

    return 0;
}