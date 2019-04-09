#include <iostream>
#include <vector>

class B{
public:
    B(){}

    B(const B & b){}
};

class A{
private:
    B * m_b;

public:
    A() : m_b(new B()){
        std::cout << "执行了构造函数A()" << std::endl;
    }

    A(const A & a) : m_b(new B(*a.m_b)){
        std::cout << "执行了拷贝构造函数" << std::endl;
    }
    
    A(A && a) : m_b(a.m_b){
        a.m_b = nullptr;
        std::cout << "执行了移动构造函数" << std::endl;
    }

    ~A(){
        delete m_b;
        std::cout << "执行了析构函数" << std::endl;
    }

    A& operator=(const A & a){
        delete m_b;
        m_b = new B(*a.m_b);
        std::cout << "执行了拷贝赋值运算符(const A & a)" << std::endl;
        return *this;
    }

    A& operator=(A & a){
        delete m_b;
        m_b = new B(*a.m_b);
        std::cout << "执行了拷贝赋值运算符(A & a)" << std::endl;
        return *this;
    }

    // A& operator=(A && a){
    //     delete m_b;
    //     m_b = a.m_b;
    //     a.m_b = nullptr;
    //     std::cout << "执行了移动赋值运算符" << std::endl;
    //     return *this;
    // }
};

int main(){
    A a;
    A a1;
    a1 = a;

    return 0;
}