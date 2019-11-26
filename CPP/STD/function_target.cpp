/**
 * 测试std::function中的target<>模板函数
*/

#include <iostream>

typedef void (*Func)();

class Test {
public:
    Test(int data) : m_data(data){}
    ~Test() = default;

    void print(){
        std::cout << "data = " << m_data << std::endl;
    }

private:
    int m_data;
};

void test_func(Func func){
    func();
}

int main(){
    Test t(10);
    std::function<void()> func = std::bind(&Test::print, &t);
    test_func(func.target<Func>());

    return 0;
}