#include "node.h"
#include <iostream>

Node::Node(){
    std::cout << "执行了父类的构造函数Node::Node()" << std::endl;
}

Node::~Node(){
    std::cout << "执行了父类的析构函数Node::~Node()" << std::endl;
}

void Node::print(){
    std::cout << "执行了父类的print()函数" << std::endl; 
}

void Node::print(int i){
    std::cout << "执行了父类的print(int i)函数" << std::endl; 
}