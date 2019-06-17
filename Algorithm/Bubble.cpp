#include <iostream>
#include "sortTestHelp.h"

template <typename T>
void bubbleSort(T * arr, T n){
    for(int i = 0; i < n-1; ++ i){
        for(int j = 0; j < n-i-1; ++ j){
            if(arr[j] > arr[j+1]){
                std::swap(arr[j], arr[j+1]);
            }
        }
    }
}

int main(){
    int n = 100;
    int arr[n];
    generateRamdomArray(arr, n, 0, n);
    bubbleSort(arr, n);
    printArray(arr, n);

    return 0;
}