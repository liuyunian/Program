/**
 * std::string::substr()用法
*/

#include <iostream>
#include <string>

int main(){
    std::string str("hello world");
    std::cout << str.substr(0, 5) << std::endl;
    std::cout << str.substr(6) << std::endl;
    std::cout << str.substr(0, str.size()-1) << std::endl;
    std::cout << str.substr(1, 2) << std::endl;
    std::cout << str.substr(0, 2) << std::endl;

    return 0;
}