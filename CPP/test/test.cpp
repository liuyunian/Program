#include <iostream>
#include <vector>

// template<typename T, typename F>
// void func(T a, F b){
//     std::cout << a << std::endl;
//     std::cout << b << std::endl;
// }

// template<typename... T>
// void func(T... args){
//     std::cout << sizeof...(args) << std::endl;
//     std::cout << sizeof...(T) << std::endl;
// }
// void func(){
//     std::cout << "参数展开结束" << std::endl;
// }

// template<typename T, typename... Args>
// void func(T first, Args... others){
//     std::cout << "展开的参数是：" << first << std::endl;

//     func(others...);
// }


template <typename T, template<class> class Container>
class A{
private:
    T m_t;
    Container<T> m_container;
public:
    A(T t) : m_t(t){
        for(int i = 0; i < m_t; i++){
            m_container.push_back(i);
        }
    }

    void print(){
        for(auto & item : m_container){
            std::cout << item << std::endl;
        }
    }
};

template <typename T>
using Vector = std::vector<T, std::allocator<T>>;


int main(){
    // func();
    // func('a');
    // func(1, 2);
    // func(1, 'a', "hello");
    // A<int, double> a(1, 1.1);
    A<int, Vector> a(5);
    a.print();

    return 0;
}