#include <iostream>

template <typename T>
class List{
private:
    class Node{
        T data;
        Node * next;

        Node(T elem) : data(elem), next(nullptr){}
    };

    Node * head;
    int count;

public: 
    List();

    inline int size(){
        return count;
    }

    inline bool empty(){
        return count == 0;
    }

    Node * getHead();
};

template <typename T> 
List<T>::List() : head(nullptr), count(0){}

template <typename T> 
typename List<T>::Node * List<T>::getHead(){
    return head;
}

int main(){

    return 0;
}
