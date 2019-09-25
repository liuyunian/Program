/**
 * 整型提升
 */
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