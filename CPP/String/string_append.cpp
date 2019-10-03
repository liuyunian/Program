/**
 * std::string::append()用法
*/

#include <iostream>
#include <string>

int main(){
    std::string str("test: ");
    std::string str2("test: ");
    std::string str3("test: ");
    std::string str4("test: ");
    std::string str5("test: ");

    std::string tmpString("hello world");
    str.append(tmpString);
    str2.append(tmpString, 0, 5);

    str3.append("liu");
    str4.append("yunian", 2);

    str5.append(5, 'a');

    std::cout << str << std::endl;
    std::cout << str2 << std::endl;
    std::cout << str3 << std::endl;
    std::cout << str4 << std::endl;
    std::cout << str5 << std::endl;

    return 0;
}