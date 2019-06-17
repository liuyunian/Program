#include <iostream>
#include <assert.h>

void generateRamdomArray(int * arr, int n, int rangeL, int rangeR){
    assert(rangeL <= rangeR);

    srand(time(NULL));
    for(int i = 0; i < n; ++ i){
        arr[i] = rand() % (rangeR-rangeL+1) + rangeL;
    }
}

void printArray(int * arr, int n){
    for(int i = 0; i < n-1; ++ i){
        std::cout << arr[i] << " ";
    }
    std::cout << arr[n-1] << std::endl;
}