#include <iostream>
#include <initializer_list>

void print(std::initializer_list<std::string> strList, int data){
    for(auto iter = strList.begin(); iter != strList.end(); ++ iter){
        std::cout << *iter << std::endl;
    }

    for(auto & item : strList){
        std::cout << item << std::endl;
    }

    std::cout << strList.size() << std::endl;
    std::cout << data << std::endl;
}

int main(){
    print({"aa", "bb", "cc"}, 1);

    return 0;
}