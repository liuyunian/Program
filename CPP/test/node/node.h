#ifndef NODE_H_
#define NODE_H_
#include <iostream>

class Node{
private:
    std::string name;

public:
    Node();
    Node(std::string n);

    void printName();

};

#endif