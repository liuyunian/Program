#include <iostream>

template <typename F>
class A{
public:
    template <typename T> 
    void func(T a);
};
template <typename F>
template <typename T> 
void A<F>::func(T a){
    std::cout << a << std::endl;
}


int main(){
    A<int> a;
    a.func("hello");

    return 0;
}