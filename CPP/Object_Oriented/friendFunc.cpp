/**
 * 友元函数
*/

#include <iostream>
#include <string>

class Test {
public:
    Test(int data, std::string name) : m_data(data), m_name(name){}

    ~Test() = default;

    friend void test_func(Test& t);

protected:
    int m_data;

private:
    std::string m_name;
};

void test_func(Test& t){
    std::cout << "data = " << t.m_data << '\n';         // protected member
    std::cout << "name = " << t.m_name << std::endl;    // private member
}

int main(){
    Test t(10, "test");
    test_func(t);

    return 0;
}

