#include <iostream>
class Node;
class A{
private:
    Node & a_node;
public:
    A(Node & node) : a_node(node){}
    // void setName(std::string name){
    //     a_node.name = name;
    // }

    void printName(){
        std::cout << a_node.name << std::endl;
    }

};

class Node{
private:
    std::string name;

public:
    Node(std::string n) : name(n){}

    // friend class A;
    friend void A::printName();
};

int main(){
    Node n("/prefix");

    A a(n);
    a.printName();
    // a.setName("/A");
    // a.printName();
}