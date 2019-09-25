/**
 * new和delete的基本用法
 */
#include <iostream>

int main(){
    // int * p1 = new int;
    // int * p2 = new int();

    // std::cout << *p1 << std::endl;
    // std::cout << *p2 << std::endl;

    double * p1 = new double;
    double * p2 = new double();

    std::cout << *p1 << std::endl;
    std::cout << *p2 << std::endl;

    return 0;
}