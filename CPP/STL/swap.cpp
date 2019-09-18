/**
 * STL中的swap()函数
*/
#include <iostream>
#include <algorithm>

int main(){
    int a = 1, b = 2;
    std::cout << "(" << a << ", " << b << ")" << std::endl;
    std::swap(a, b);
    std::cout << "(" << a << ", " << b << ")" << std::endl;
    
    return 0;
}