#include <iostream>
#include <vector>

class A{
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

    A & operator=(const A & a){
        m_value_a = a.m_value_a;
        std::cout << "执行了拷贝复制运算符" << std::endl;
        return *this; 
    }

    explicit operator int() const {
        std::cout << "执行了类型转换运算符" << std::endl;

        return this->m_value_a;
    }
};



int main(){
    A a(1);
    // a = 1;
    // int sum = a + 2;
    int sum1 = a.operator int() + 2;
    int sum2 = static_cast<int>(a) + 2;
    std::cout << sum2 << std::endl;
    return 0;
}