/**
 * std::string::find()用法
*/

#include <iostream>
#include <string>

int main(){
    std::string str("hello world");

    std::string tmpString("he");
    size_t n = str.find(tmpString);
    std::cout << n << std::endl;

    n = str.find("world");
    std::cout << n << std::endl;

    n = str.find("liu");
    std::cout << n << std::endl;
    if(n == std::string::npos){
        std::cout << "not find" << std::endl;
    }

    n = str.find('l');
    std::cout << n << std::endl;

    return 0;
}