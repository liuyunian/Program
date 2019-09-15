#include <string>
#include <iostream>

void print(const std::string & str){
    std::cout << str << std::endl;
}

int main(){
    std::string string_cpp = "1";
    print(string_cpp);

    char str_c[] = "2";
    print(str_c);

    const char * str_cost = "3";
    print(str_cost);

    return 0;
}