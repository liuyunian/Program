## 枚举enum基础
之前使用枚举类型都是用来代替#define来定义常量，因为使用enum定义常量可以让一组有关系的常量彼此之间有关联性，这样比#define更具有可读性  
其实可以使用枚举类型定义对象，类似于struct结构定义结构对象  
**markdown换行是：空格+空格+回车**
```c++
enum Log_level{
    NGX_LOG_FATAL,  // [fatal] -- 致命错误
    NGX_LOG_ERR,    // [error] -- 错误
    NGX_LOG_WARN,   // [warn]  -- 警告
    NGX_LOG_INFO,   // [info]  -- 提示
    NGX_LOG_DEBUG   // [debug] -- 调试
};
```
枚举中的成员默认都是赋予整形值，如果不进行赋值那么第一个成员的值为0，后面的成员以此+1
也可以对成员进行赋值
```C++
enum Log_level{
    NGX_LOG_FATAL = 0,  
    NGX_LOG_ERR = 1,
    NGX_LOG_WARN = 2,
    NGX_LOG_INFO = 4,
    NGX_LOG_DEBUG = 8
}
```

可以定义Log_level类型的对象，该对象的特点是：取值只能是枚举Log_level中定义的值 

```C++
Log_level level1(NGX_LOG_FATAL); // 初始化方式1
Log_level level2 = NGX_LOG_ERR; // 初始化方式2

Log_level level3;
level3 = NGX_LOG_WARN; // 赋值
```
枚举对象初始化有如上两种方式，还可以在定义之后进行赋值，但是需要注意的是：取值只能是枚举Log_level中定义的值

## 整型提升
什么是整型提升？  
首先应该明确：
* 在32位机器上：char占1字节、short占2字节、int占4字节、long占4字节、longlong占8字节、float占4字节、double占8字节、指针占4字节
* 在64位机器上：char占1字节、short占2字节、int占4字节、long占8字节、longlong占8字节、float占4字节、double占8字节、指针占8字节  

整型提升是指：char类型、short类型、位域、枚举量都可以用在int或者unsigned int场合
```C++
#include <iostream>

enum Color{
    red,
    yello,
    green,
    blue
};

void print(int data){
    std::cout << "data = " << data << std::endl;
}

int main(){
    char ch = 'a';
    print(ch);

    short sh = 1;
    print(sh);

    Color cl(red);
    print(cl);

    return 0;
}
```
可以看到在调用print()函数时发生了整型提升

整型提升的原理
填充符号位
* 如果是有符号数，正数填充0，负数填充1
* 无符号数填充0

## enum存在的问题

## enum class/struct
https://www.cnblogs.com/diegodu/p/6169808.html