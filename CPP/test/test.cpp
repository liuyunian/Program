#include <iostream>
#include <vector>
#include <map>
#include <functional>

class A{
private:
    int m_value_a;

public:
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

    // explicit operator int() const {
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

int show(){
    return 1;
}

int main(){
    // A a;
    int (*p)(int) = show;
    std::function<int(int)> func = (*p);
    func(1);
    // std::map<std::string, std::function<int(int)>> m_opt = {
    //     {"show", show}
    //     // {"A_show", a},
    //     // {"A_show2", A()}
    // };

    // m_opt["show"](1);
    // m_opt["A_show"](1);
    // m_opt["A_show2"](1);
    // std::map<std::string, std::function<int(int)>> opt;

    return 0;
}