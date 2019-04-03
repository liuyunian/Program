#ifndef DISTNODE_H_
#define DISTNODE_H_
#include "node.h"

class DistNode : public Node{
private:
    int m_data;

public:
    DistNode(int data);

    ~DistNode();

    std::string getName();

    void printName();

    int getData();
};

#endif