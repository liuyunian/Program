#include <iostream>
#include <cstdio>
#include <functional>
// void test(int a, int b, int c){
//     std::cout << "a = " << a << std::endl;
//     std::cout << "b = " << b << std::endl;
//     std::cout << "c = " << c << std::endl;
// }

// void test2(int & a, int & b){
//     ++ a;
//     ++ b;
// }

// class A{
// public:
//     int m_data = 0;

// public:
//     A(){
//         std::cout << "执行了默认构造函数" << std::endl;
//     }

//     A(const A & a) : m_data(a.m_data){
//         std::cout << "执行了拷贝构造函数" << std::endl;
//     }

//     void m_func(int a, int b){
//         std::cout << "执行了成员函数m_func(int a, int b)" << std::endl;
//         m_data = a;
//     }

//     int operator()(int data){
//         std::cout << "执行了函数调用运算符" << std::endl;

//         return m_data;
//     }
// };

// int main(){
//     // auto func = std::bind(test, 1, 2, 3);
//     // func();

//     // auto func2 = std::bind(test, std::placeholders::_1, std::placeholders::_2, 30);
//     // func2(10, 20);

//     // int a = 1, b = 2;
//     // auto func3 = std::bind(test2, std::placeholders::_1, b);
//     // func3(a);
//     // std::cout << a << ", "<< b << std::endl;

//     // A * a = new A();
//     // auto func3 = std::bind(&A::m_func, a, std::placeholders::_1, 3);
//     // func3(2);
//     // std::cout << a->m_data << std::endl;

//     // A a;
//     // auto func4 = std::bind(a, 1);
//     // func4();

//     std::function<void(int, int)> func = std::bind(test, std::placeholders::_1, std::placeholders::_2, 30);
//     func(10, 20);

//     return 0;
// }

void print(int data){
    std::cout << data << std::endl;
}

void testFunc(int data, const std::function<void(int)> & func){
    func(data);
}

int main(){  
    for(int i = 0; i < 10; ++ i){
        testFunc(i, std::bind(print, std::placeholders::_1));
    }
    return 0;
}
