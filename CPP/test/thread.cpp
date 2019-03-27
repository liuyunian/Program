#include <iostream>
#include <thread>

class A{
public:
    A(){}

    A(const A & a){
        // std::cout << "线程ID = " << this_thread
        std::cout << "执行了拷贝构造函数" << std::endl;
    }

    void operator()(){
        std::cout << "child thread" << std::endl; 
    }

};

int main(){
    A a;
    std::thread t(a);
    // t.join();
    return 0;
}
