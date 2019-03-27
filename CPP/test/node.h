#ifndef NODE_H_
#define NODE_H_

#include <string>
#include <iostream>

class Node{
private:
    std::string n_name;

public:
    Node(){}
    

    Node(std::string name);

    void getName(){
        std::cout << "Node::getName()" << std::endl;
    }

    void getName(int a){
        std::cout << "Node::getName(int a)" << std::endl;
    }
};

#endif