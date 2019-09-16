#include <iostream>
#include <memory>

class Test{
public:
    Test(){
        std::cout << "执行了Test()" << std::endl;
    }

    ~Test(){
        std::cout << "执行了~Test()" << std::endl;
    }
};

class Foo{
public:
    Foo(){
        std::cout << "执行了Foo()" << std::endl;
    }

    ~Foo(){
        std::cout << "执行了~Foo()" << std::endl;
    }
}; 

void testFunc(std::shared_ptr<void> arg){

}

int main(){
    auto p_test = std::make_shared<Test>();
    auto p_foo = std::make_shared<Foo>();
    testFunc(p_test);
    testFunc(p_foo);

    return 0;
}