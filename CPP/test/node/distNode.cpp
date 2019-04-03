#include "distNode.h"
#include <iostream>

DistNode::DistNode(int data) : m_data(data){
    std::cout << "执行了子类的构造函数DistNode::DistNode(int data)" << std::endl;
}

DistNode::~DistNode(){
    std::cout << "执行了子类的析构函数DistNode::~DistNode()" << std::endl;
}

int DistNode::getData(){
    return this->m_data;
}

std::string DistNode::getName(){
    std::cout << "执行了子类的getName()" << std::endl;
}

void DistNode::printName(){
    std::cout << Node::getName() << std::endl;
}