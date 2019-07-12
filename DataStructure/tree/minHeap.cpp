#include <iostream>
#include <cassert>
#include "testHelper.cpp"

template <typename T>
class MinHeap{
private:
    T * data;
    int count;
    int capacity;

public:
    MinHeap(int cap) : data(new T[cap+1]), count(0), capacity(cap){}

    MinHeap() : data(new T[11]), count(0), capacity(10){}

    MinHeap(T arr[], int n) : data(new T[n+1]), count(n), capacity(n){
        for(int i = 0; i < n; i++){
            data[i+1] = arr[i];
        }

        for(int i = count/2; i >= 1; i--){
            shiftDown(i);
        }
    }

    int size(){
        return count;
    }

    bool empty(){
        return count == 0;
    }

    int getCapacity(){
        return capacity;
    }

    void add(T elem){
        if(count == capacity){
            resize(2*capacity);
        }

        data[count+1] = elem;
        count ++;
        shiftUp(count);
    }

    T extractMin(){
        assert(count > 0);

        T elem = data[1];
        data[1] = data[count];
        count --;
        if(count == capacity/4 && capacity/2 >= 10){
            resize(capacity/2);
        }

        shiftDown(1);
        return elem;
    }

    void printMinHeap(){
        std::cout << "MinHeap: size = " << this->size() << " capacity = " << this->getCapacity() << std::endl;
        std::cout << "[";
        for(int i = 1; i < count; i++){
            std::cout << data[i] << " ";
        }
        std::cout << data[count] << "]" << std::endl;
    }

private:
    void resize(int newCapacity){
        T * newData = new T[newCapacity+1];

        for(int i = 1; i<=count; i++){
            newData[i] = data[i];
        }

        data = newData;
        capacity = newCapacity;
    }

    void shiftUp(int index){
        while(index > 1 && data[index/2] > data[index]){
            std::swap(data[index/2], data[index]);
            index = index/2;
        }
    }

    void shiftDown(int index){
        while(index * 2 <= count){
            int j = index * 2;

            if(j+1 <= count && data[j+1] < data[j]){
                j = j+1;
            }

            if(data[index] < data[j]){
                break;
            }

            std::swap(data[index], data[j]);
            index = j;
        }
    }
};

void heapSort(int arr[], int n){
    MinHeap<int> heap2(n);
    for(int i = 0; i < n; i++){
        heap2.add(arr[i]);
    }

    for(int i = 0; i < n; i++){
        arr[i] = heap2.extractMin();
    }
}

void heapSort2(int arr[], int n){
    MinHeap<int> heap(arr, n);

    for(int i = 0; i < n; i++){
        arr[i] = heap.extractMin();
    }
}

int main(){
    int n = 1000;
    int arr[n];
    generateRandom(arr, n, 0, n);
    heapSort2(arr, n);
    printArray(arr, n);

    // MinHeap<int> hp;
    // for(int i = 15; i >= 0; i--){
    //     hp.add(i);
    //     // hp.printMinHeap();
    // }

    // for(int i = 15; i >= 0; i--){
    //     std::cout << hp.extractMin() << " ";
    // }
    // std::cout << std::endl;
    return 0;
}