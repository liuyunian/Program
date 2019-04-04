#include <iostream>
#include <cassert>

class UnionFind4{
private:
    int * parents;
    int * rank;
    int count;

public:
    UnionFind4(int n) : count(n){
        parents = new int[n];
        rank = new int[n];

        for(int i = 0; i < n; i++){
            parents[i] = i;
            rank[i] = 1;
        }
    }

    ~UnionFind4(){
        delete [] parents;
        delete [] rank;
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

        if(rank[pRoot] > rank[qRoot]){
            parents[qRoot] = pRoot;
        }
        else if(rank[pRoot] < rank[qRoot]){
            parents[pRoot] = qRoot;
        }
        else{
            parents[pRoot] = qRoot;
            rank[qRoot] ++;
        }
    }
};