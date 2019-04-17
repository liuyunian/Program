#include <iostream>
#include <functional>
#include <algorithm>
#include <vector>

// class A{
// private:
//     int m_data = 2;

// public:
//     void testFunc(int x, int y){
//         auto func = [this]{
//             std::cout << m_data << std::endl;
//             std::cout << "(" << x << "," << y << std::endl;
//         };
//         func();
//     }
// };

// void print(int val){
//     std::cout << val << std::endl;
// }

// std::function<void()> func;

// void test(){
//     int a = 5;
//     func = [&a]{
//         std::cout << a << std::endl;
//     };
// }

std::function<void()> func;
class A{
private:
    int m_data = 2;
public:
    void testFunc(int x, int y){
        func = [a = m_data]{
            std::cout << a << std::endl;
        };
    }
};


int main(){
    // std::function<int(int)> func(
    //     [](int data) -> int{
    //         std::cout << "执行了lambda表达式" << std::endl;
    //         return data*2;
    //     }
    // );

    // std::cout << func(10) << std::endl;

    // auto func = std::bind(
    //     [](int data) -> int{
    //         std::cout << "执行了lambda表达式" << std::endl;
    //         return data*2;
    //     },
    //     10
    // );
    // std::cout << func() << std::endl;

    // int (*func)(int);
    // func = [](int data) -> int {
    //     std::cout << "执行了lambda表达式" << std::endl;
    //     return data*2;
    // };
    // std::cout << func(10) << std::endl;
    // std::vector<int> data{10, 20, 30};
    // // std::for_each(data.begin(), data.end(), print);
    // int sum = 0;
    // std::for_each(data.begin(), data.end(), [&sum](int val){
    //     sum += val;
    //     std::cout << val << std::endl; 
    // });
    // std::cout <<"sum = " << sum << std::endl;


    // test();
    // func();

    // auto func = [](auto data){
    //     std::cout << data << std::endl;
    // };
    // func(12);

    // A * a = new A();
    // a->testFunc(10, 20);
    // delete a;
    // func(); //执行出错，对象a已经被销毁了，不能再访问a对象的成员m_data
    

    return 0;
}