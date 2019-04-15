#include <iostream>

int main(){
    auto func = []() -> void {
        std::cout << "执行了lambda表达式" << std::endl;
        // return 1;
    };

    func();

    // std::cout << func() << std::endl;

    // []{std::cout << "hello world" << std::endl;}();

    return 0;
}