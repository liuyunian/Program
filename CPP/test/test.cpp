#include <iostream>

int main(){
    std::string str("hello");
    std::string str2 = std::move(str);
    std::cout << str << std::endl;
    std::cout << str2 << std::endl;
    return 0;
}