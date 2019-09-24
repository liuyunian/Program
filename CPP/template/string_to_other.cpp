/**
 * 将std::string类型转化成其他类型（包括std::string）
*/

#include <iostream>
#include <sstream>

template<typename T>
T convert(std::string & string){
    T retVal;
    std::stringstream(string) >> retVal;

    return retVal;
}

int main(){
    std::string string("liuyunian");
    std::string string_int("8080");
    std::string string_double("3.14");

    std::cout << convert<std::string>(string) << std::endl;

    int port = convert<int>(string_int);
    std::cout << port << std::endl;

    double pi = convert<double>(string_double);
    std::cout << pi << std::endl;

    return 0;
}