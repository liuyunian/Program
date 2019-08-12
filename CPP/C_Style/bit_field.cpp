#include <stdio.h>
#include <string.h>

/*
struct
{
  unsigned int age : 3;
} Age;
 
int main(){
   Age.age = 4;
   printf( "Sizeof( Age ) : %d\n", sizeof(Age) );
   printf( "Age.age : %d\n", Age.age );
 
   Age.age = 7;
   printf( "Age.age : %d\n", Age.age );
 
   Age.age = 8; // 二进制表示为 1000 有四位，超出-----------会有警告信息
   printf( "Age.age : %d\n", Age.age );
 
   return 0;
}

 */

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
 
int main(){
   printf( "Memory size occupied by status1 : %lu\n", sizeof(status1));
   printf( "Memory size occupied by status2 : %lu\n", sizeof(status2));
   printf( "Memory size occupied by status3 : %lu\n", sizeof(status3));
 
   return 0;
}