#include "node.h"
#include <iostream>

Node::Node(std::string name) : m_name(name){
    std::cout << "执行了父类的构造函数Node::Node(std::string name)" << std::endl;
}

Node::~Node(){
    std::cout << "执行了父类的析构函数Node::~Node()" << std::endl;
}

std::string Node::getName(){
    std::cout << "执行了父类的getName()" << std::endl;
    return this->m_name;
}