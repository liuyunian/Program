#include "node.h"

Node::Node(){}

Node::Node(std::string n) : name(n){}

void Node::printName(){
    std::cout << "父类节点的名称" << this->name << std::endl;
}