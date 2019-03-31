#include "distNode.h"
#include <iostream>

DistNode::DistNode(){
    std::cout << "执行了子类的构造函数DistNode::DistNode()" << std::endl;
}

DistNode::~DistNode(){
    std::cout << "执行了子类的析构函数DistNode::~DistNode()" << std::endl;
}

void DistNode::print(std::string str){
    // Node::print();
    // Node::print(1);
    std::cout << "执行了子类的print(std::string str)函数" << std::endl;
}