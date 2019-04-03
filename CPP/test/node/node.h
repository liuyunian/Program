#ifndef NODE_H_
#define NODE_H_
#include <iostream>

class Node{
private:
    std::string m_name;

public:
    Node();

    virtual ~Node();

    void setName(std::string name);

    virtual std::string getName();

    virtual void printName() = 0;
};

#endif