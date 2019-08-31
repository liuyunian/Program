/**
 * 使用muduo库的Timestamp类
*/

#include <iostream>

#include "muduo/base/Timestamp.h"

int main(){
    Timestamp now(Timestamp::now()); // 拷贝
    
    std::cout << now.toString() << std::endl;
    std::cout << now.toFormattedString(true) << std::endl;

    return 0;
}