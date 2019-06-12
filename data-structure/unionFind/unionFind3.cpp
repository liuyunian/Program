#include <iostream>
#include <cassert>

class UnionFind3{
private:
    int * parents;
    int * size;
    int count;

public:
    UnionFind3(int n) : count(n){
        parents = new int[n];
        size = new int[n];

        for(int i = 0; i < n; i++){
            parents[i] = i;
            size[i] = 1;
        }
    }

    ~UnionFind3(){
        delete [] parents;
        delete [] size;
    }

    int find(int p){
        assert(p >= 0 && p < count);

        while(p != parents[p]){
            p = parents[p];
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

        if(size[pRoot] > size[qRoot]){
            parents[qRoot] = pRoot;
            size[pRoot] += size[qRoot];
        }
        else{
            parents[pRoot] = qRoot;
            size[qRoot] += size[pRoot];
        }
    }
};