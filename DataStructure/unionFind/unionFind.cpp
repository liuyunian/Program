#include <iostream>
#include <cassert>

class UnionFind{
private:   
    int * id;
    int count;

public:
    UnionFind(int n){
        count = n;
        id = new int[n];
        for(int i = 0; i < n; i++){
            id[i] = i; //每一个元素的集合都不一样
        }
    }

    ~UnionFind(){
        delete [] id;
    }

    int find(int p){ //查找元素p所在的集合
        assert(p >=0 && p < count);
        
        return id[p];
    }

    bool isConnected(int p, int q){
        return find(p) == find(q);
    }

    void unionElements(int p, int q){
        int pID = find(p);
        int qID = find(q);

        if(pID == qID){ //已经在一个集合里了
            return;
        }

        for(int i = 0; i < count; i++){
            if(id[i] == qID){
                id[i] = pID;
            }
        }
    }
};