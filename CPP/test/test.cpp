#include <iostream>
#include <vector>

int main(){
    int lineNum;
    std::cin >> lineNum;
    std::cin.ignore(); // 忽略缓存区的回车符

    std::vector<std::string> inputs;
    std::string input;
    for(int i = 0; i < lineNum; i++){
        std::getline(std::cin, input);
        inputs.push_back(input);
    }

    for(auto &item : inputs){
        std::cout << item << std::endl;
    }
    return 0;
}