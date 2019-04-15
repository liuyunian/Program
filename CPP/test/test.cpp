#include <iostream>
#include <vector>
#include <map>
#include <functional>

class A{
private:
    int m_value_a;

public:
    // using func_ptr = void(*)(int);

    // static void myfunc(int data){
    //     std::cout << "执行了静态函数myfunc" << std::endl;
    //     std::cout << data << std::endl;
    // }

    // operator func_ptr(){
    //     std::cout << "执行了类对象转换成函数指针的类型转换运算符函数" << std::endl;

    //     return myfunc;
    // }

    A(){
        // std::cout << "执行类A的默认构造函数" << std::endl;
    }

    A(int value) : m_value_a(value){
        std::cout << "执行类A的构造函数A(int value)" << std::endl;
    }

    ~A(){
        // std::cout << "执行类A的析构函数" << std::endl;
    }

    A & operator=(const A & a){
        m_value_a = a.m_value_a;
        std::cout << "执行了拷贝复制运算符" << std::endl;
        return *this; 
    }

    // operator int() const {
    //     std::cout << "执行了类型转换运算符" << std::endl;

    //     return this->m_value_a;
    // }

    int operator()(int data) const {
        std::cout << "执行了函数调用运算符" << std::endl;

        return data;
    }
};

int show(int data){
    std::cout << "执行了show普通函数" << std::endl;

    return data;
}

int main(){
    A a;
    std::function<int(int)> func(show);
    std::function<int(int)> func2(a);
    // func(1);
    // func2(20);
    std::map<std::string, std::function<int(int)>> opt = {
        {"show", func},
        {"a_show", func2}
    };

    opt["show"](1);
    opt["a_show"](1);
    

    return 0;
}