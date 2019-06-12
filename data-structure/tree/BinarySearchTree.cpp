#include <iostream>
#include <cassert>

template <typename Key, typename Value>
class BinarySearchTree{
private: 
    struct Node{
        Key key;
        T value;
        Node * left, * right;

        Node(Key k, T v) : key(k), value(v), left(nullptr), right(nullptr){}

        Node(Node * n) : key(n->key), value(n->value), left(n->left), right(n->right){} //拷贝Node指针指向的结点
    };

    Node * root;
    int count;

public:
    BinarySearchTree() : root(nullptr), count(0){}

    ~BinarySearchTree(){
        destory(root);
    }

    int size(){
        return count;
    }

    bool empty(){
        return count == 0;
    }

    void insert(key key, Value value){
        root = insert(root, key, value);
    }

    bool contain(Key key){
        return contain(root, key);
    }

    Value search(Key key){
        return search(root, key);
    }

    Key getMin(){
        assert(count > 0);

        Node * minNode =  minimum(root);
        return minNode->key;
    }

    Key getMax(){
        assert(count > 0);

        Node * maxNode = maximum(root);
        return maximum(root);
    }

    void removeMin(){
        assert(count > 0);

        root = removeMin(root);
    }

    void removeMax(){
        assert(count > 0);

        root = removeMax(root);
    }

    void remove(Key key){
        assert(count > 0);

        root = remove(root, key);
    }

private:
    Node * insert(Node * node, Key key, Value value){
        if(node == nullptr){
            count ++;
            return new Node(key, value);
        }

        if(node->key == key){
            node->value = value;
        }
        else if(node->key > key){
            node->left = insert(node->left, key, value);
        }
        else{
            node->right = insert(node->right, key, value);
        }

        return node;
    }

    bool * contain(Node * node, Key key){
        if(node == nullptr){
            return false;
        }

        if(node->key == key){
            return true;
        }
        else if(node->key > key){
            return contain(node->left, key);
        }
        else{
            return contain(node->right, key);
        }
    }

    Value search(Node * node, Key key){
        if(node->key == key){
            return node->value;
        }
        else if(node->key > key){
            return search(node->left, key);
        }
        else{
            return search(node->right, key);
        }
    }

    void destory(Node * node){
        if(node == nullptr){
            return;
        }

        destory(node->left);
        destory(node->right);
        delete node;
    }

    Node * minimum(Node * node){
        if(node->left == nullptr){
            return node;
        }
        
        return minimum(node->left);
    }

    Node * maximum(Node * node){
        if(node->right == nullptr){
            return node;
        }

        return maximum(node->right);
    }

    Node * removeMin(Node * node){
        if(node->left == nullptr){
            Node * rightChild = node->right;
            delete node;
            count --;
            return rightChild;
        }

        node->left = removeMin(node->left);
        return node;
    }

    Node * removeMax(Node * node){
        if(node->right == nullptr){
            Node * leftChild = node->left;
            delete node;
            count --;
            return leftChild;
        }

        node->right = removeMax(node->right);
        return node;
    }

    Node * remove(Node * node, Key key){
        if(node == nullptr){ //没有找到是不是要通知函数调用者？
            return nullptr;
        }

        if(node->key > key){
            node->left = return remove(node->left, key);
            return node;
        }

        else if(node->key < key){
            node->right = return remove(node->right, key);
            return node;
        }

        else{ //找到了键值为key的结点
            if(node->left == nullptr){
                Node * rightChild = node->right;
                delete node;
                count --;
                return rightChild;
            }

            if(node->right == nullptr){
                Node * leftChild = node->left;
                delete node;
                count --;
                return leftChild;
            }

            if(node->left != nullptr && node->right != nullptr){
                Node * successor = new Node(minimum(node->right));
                count ++;
                successor->right = removeMin(node->right);
                successor->left = node->left;

                delete node;
                count --;
                return successor;
            }
        }
    }
};