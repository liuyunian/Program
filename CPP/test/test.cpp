#include <iostream>
#include <vector>

class A{
public:
    int m_value1;
    int m_value2 = 0;

public:
    A(int v1 = 0, int v2 = 0) : m_value1(v1), m_value2(v2){
        std::cout << "执行了构造函数A(int v1 = 0, int v2 = 0)" << std::endl;
        std::cout << "(" << m_value1 << ", " << m_value2 << ")" << std::endl;
    }

    // A(int v1) : m_value1(v1){
    //     std::cout << "执行了构造函数A(int)" << std::endl;
    //     std::cout << "(" << m_value1 << ", " << m_value2 << ")" << std::endl;
    // }

    A(const A & a) : m_value1(a.m_value1), m_value2(a.m_value2){
        std::cout << "执行了拷贝构造函数" << std::endl;
        std::cout << "(" << m_value1 << ", " << m_value2 << ")" << std::endl;
    }

    ~A(){
        std::cout << "执行了析构函数" << std::endl;
        std::cout << "(" << m_value1 << ", " << m_value2 << ")" << std::endl;
    }

    A & operator=(const A & a){
        m_value1 = a.m_value1;
        m_value2 = a.m_value2;

        std::cout << "执行了重载赋值运算符" << std::endl;
        std::cout << "(" << m_value1 << ", " << m_value2 << ")" << std::endl;
    }

    // void add(A & a){
    //     m_value1 += a.m_value1;
    //     m_value2 += a.m_value2;
    //     std::cout << "(" << m_value1 << ", " << m_value2 << ")" << std::endl;

    //     a.m_value1 = 1000;
    // }
};

A Double(A & a){
    A a_tmp;
    a_tmp.m_value1 = a.m_value1*2;
    a_tmp.m_value2 = a.m_value2*2;

    return a_tmp;    
}

int main(){
    A a(1, 2);
    A a2 = Double(a);
    std::cout << "main function" << std::endl;

    return 0;
}