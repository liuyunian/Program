#include <iostream>

void generateRandom(int arr[], int n, int left, int right){
    srand(time(NULL));

    for(int i = 0; i < n; i++){
        arr[i] = rand()%(right-left)+left;
    }
}

void printArray(int arr[], int n){
    for(int i = 0; i < n; i++){
        std::cout << arr[i] << " ";
    }
}