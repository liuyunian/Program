/**
 * STL中的iter_swap()函数
 * 用于交换两个迭代器
*/

#include <iostream>
#include <vector>
#include <algorithm>

int main(){
    std::vector<int> vList = {1, 2};
    for(auto & item : vList){
        std::cout << item << " ";
    }
    std::cout << std::endl;

    auto iter_a = vList.begin();
    auto iter_b = vList.end() - 1;
    // std::swap(iter_a, iter_b);   // 没有起到交换的作用
    std::iter_swap(iter_a, iter_b);

    for(auto & item : vList){
        std::cout << item << " ";
    }
    std::cout << std::endl;

    return 0;
}