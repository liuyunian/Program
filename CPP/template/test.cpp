#include <iostream>

template<typename T, int b>
T add(T a){
    T sum = a+b;
    return sum;
}

int main(){
    // int result = add(1, 2);
    // double result  = add<double>(1.2l, 3.2l);
    const int c = 1;
    int result = add<int, c>(4);
    std::cout << result << std::endl;

    return 0;
}