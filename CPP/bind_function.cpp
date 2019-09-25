/**
 * bind & function
*/
#include <map>
#include <iostream>
#include <functional>

typedef std::function<void(char)> Callback;

void exec_callback(Callback cb){
    cb(10);
}

class Test{
public:
    Test() = default;

    ~Test() = default;

    void handle(){
        std::cout << "handle()" << std::endl;
    }
};

void testFunc(){
    std::cout << "testFunc" << std::endl;
}

int main(){
    Test t;
    exec_callback(std::bind(&Test::handle, &t));    // 为什么不符合函数类型的函数也能适配呢？？？
    exec_callback(std::bind(testFunc));

    return 0;
}