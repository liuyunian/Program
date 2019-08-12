## 位域
首先明确一点：位域用在结构中

结构中带有预定义宽度的整型变量称为位域，注意是整型变量
声明格式如下：
```C++
struct{
    type name : width;
};

struct Age
{
  unsigned int age : 3;
};
```
位域主要有两个好处
* 一定的情况下可以节省空间
* 限制结构中成员的取值范围，如果超过指定宽度所限制的范围，编译器会警告，这样有助于程序排错

一个位域节省空间的例子
```C++
/* 定义简单的结构 */
struct{
  unsigned int widthValidated;
  unsigned int heightValidated;
} status1;
 
/* 定义位域结构 */
struct{
  unsigned int widthValidated : 1;
  unsigned int heightValidated : 1;
} status2;

/* 定义bool类型结构 */
struct{
  bool widthValidated;
  bool heightValidated;
} status3;

printf( "Memory size occupied by status1 : %lu\n", sizeof(status1));
printf( "Memory size occupied by status2 : %lu\n", sizeof(status2));
printf( "Memory size occupied by status3 : %lu\n", sizeof(status3));

//　执行结果
Memory size occupied by status1 : 8
Memory size occupied by status2 : 4
Memory size occupied by status3 : 2
```
可见定义位域了结构节省了空间，但是如果结构中定义开关量成员时，使用bool类型要比位域更加节省空间