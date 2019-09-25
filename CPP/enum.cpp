/**
 * 枚举
 */

#include <iostream>

enum Log_level{
    NGX_LOG_FATAL,  // [fatal] -- 致命错误
    NGX_LOG_ERR,    // [error] -- 错误
    NGX_LOG_WARN,   // [warn]  -- 警告
    NGX_LOG_INFO,   // [info]  -- 提示
    NGX_LOG_DEBUG   // [debug] -- 调试
};

int main(){
    Log_level level(NGX_LOG_FATAL);
    std::cout << level << std::endl;

    bool flag = true;
    std::cout << flag << std::endl;
    std::cout << std::boolalpha << flag << std::endl;

    return 0;
}