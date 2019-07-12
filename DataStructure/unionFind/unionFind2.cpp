#include <iostream>
#include <cassert>

class UnionFind2{
private:   
    int * parent;
    int count;

public:
    UnionFind2(int n) : count(n){
        parent = new int[n];
        for(int i = 0; i < n; i++){
            parent[i] = i; //初始化是每个元素都没有父结点，所以都指向自身
        }
    }

    ~UnionFind2(){
        delete [] parent;
    }

    int find(int p){
        assert(p >= 0 && p < count);

        while(p != parent[p]){
            p = parent[p];
        }

        return p;
    }

    bool isConnected(int p, int q){
        return find(p) == find(q);
    }

    void unionElements(int p, int q){
        int pRoot = find(p);
        int qRoot = find(q);

        if(pRoot == qRoot){
            return;
        }

        parent[pRoot] = qRoot;
    }
};