#include <iostream>
#include <vector>

class B{
private:
    int m_value_b;
public:
    B(){
        std::cout << "执行类B的默认构造函数" << std::endl;
    }

    B(int value) : m_value_b(value){
        std::cout << "执行类B的构造函数B(int value)" << std::endl;
    }

    virtual ~B(){
        std::cout << "执行类B的析构函数" << std::endl;
    }

    void myinfor(){
        std::cout << m_value_b << std::endl;
    }
};

class Grand{
public:
    int m_value_grand;

public:
    Grand(){
        std::cout << "执行类Grand的默认构造函数" << std::endl;
    }

    Grand(int value) : m_value_grand(value){
        std::cout << "执行类Grand的构造函数Grand(int value)" << std::endl;
    }

    ~Grand(){
        std::cout << "执行类Grand的析构函数" << std::endl;
    }
};

class A : virtual public Grand{
private:
    int m_value_a;

public:
    A(){
        std::cout << "执行类A的默认构造函数" << std::endl;
    }

    A(int value) : m_value_a(value){
        std::cout << "执行类A的构造函数A(int value)" << std::endl;
    }

    ~A(){
        std::cout << "执行类A的析构函数" << std::endl;
    }

    void myinfor(){
        std::cout << m_value_a << std::endl;
    }
};

class A2 : virtual public Grand{
private:
    int m_value_a2;

public:
    A2(){
        std::cout << "执行类A2的默认构造函数" << std::endl;
    }

    A2(int value) : m_value_a2(value){
        std::cout << "执行类A2的构造函数A2(int value)" << std::endl;
    }

    ~A2(){
        std::cout << "执行类A2的析构函数" << std::endl;
    }

    void myinfor(){
        std::cout << m_value_a2 << std::endl;
    }
};

class C : public A, public A2, public B{
private:
    int m_value_c;

public:
    C(){
        std::cout << "执行类A的默认构造函数" << std::endl;
    }

    C(int i, int j, int k, int l) : Grand(i), A(i), B(j), A2(k), m_value_c(l){
        std::cout << "执行类C的构造函数C(int i, int j, int k, int l)" << std::endl;
    }

    ~C(){
        std::cout << "执行类C的析构函数" << std::endl;
    }

    void myinforC(){
        std::cout << m_value_c << std::endl;
    }

    void myinfor(){
        A::myinfor();
        B::myinfor();
    }
};

int main(){
    C c(1, 2, 3, 4);
    std::cout << c.m_value_grand << std::endl;
    return 0;
}