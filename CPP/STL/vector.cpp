#include <iostream>
#include <vector>

int main(){
    std::vector<int> v{0, 1, 2};

    std::vector<int> v2 = v;            // 测试拷贝构造函数
    for(auto& item : v2){
        std::cout << item << std::endl;
    }

    std::vector<int> v3;
    v3 = v;                             // 测试赋值运算符函数
    for(auto& item : v3){
        std::cout << item << std::endl;
    }

    return 0;
}