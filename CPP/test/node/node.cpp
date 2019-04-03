#include "node.h"
#include <iostream>

Node::Node(){
    std::cout << "执行了父类的构造函数Node::Node()" << std::endl;
}

Node::~Node(){
    std::cout << "执行了父类的析构函数Node::~Node()" << std::endl;
}

void Node::setName(std::string name){
    this->m_name = name;
}

std::string Node::getName(){
    // std::cout << "执行了父类的getName()" << std::endl;
    return this->m_name;
}