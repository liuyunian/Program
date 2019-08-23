/**
 * 测试在使用迭代器遍历list容器时，删除其中的特定元素
 * 对于list容器erase只影响当前的iterator，所以下面的程序是没有问题的
*/

#include <iostream>
#include <list>

int main(){
    std::list<int> list_t = {0, 1, 2, 3, 4, 5, 6, 7, 8 ,9};

    for(auto & item : list_t){
        std::cout << item << " ";
    }

    std::cout << std::endl;

    for(auto iter = list_t.begin(); iter != list_t.end(); ++ iter){
        if(*iter % 2 == 1){
            list_t.erase(iter);
        }
    }

    for(auto & item : list_t){
        std::cout << item << " ";
    }
    std::cout << std::endl;

    return 0;
}