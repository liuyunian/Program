#include <iostream>
#include <cassert>
#include "testHelper.cpp"

template <typename T>
class MaxHeap{
private:
    T * data;
    int count;
    int capacity;

public:
    MaxHeap(int cap) : data(new T[capacity+1]), count(0), capacity(cap){}

    MaxHeap() : data(new T[11]), count(0), capacity(10){}

    MaxHeap(T arr[], int n) : data(new T[n+1]), count(n), capacity(n){
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

    T extractMax(){
        assert(count > 0);

        T elem = data[1];
        data[1] = data[count];
        count--;
        shiftDown(1);

        return elem;
    }

    void printMaxHeap(){
        std::cout << "MaxHeap: size = " << this->size() << " capacity = " << this->getCapacity() << std::endl;

        std::cout << "[";
        for(int i = 1; i < count; i++){
            std::cout << data[i] << " ";
        }
        std::cout << data[count] << "]" << std::endl;
    }

private:
    void resize(int newCapacity){
        T * newData = new T[newCapacity];

        for(int i = 1; i <= count; i++){
            newData[i] = data[i];
        }

        data = newData;
        capacity = newCapacity;
    }

    void shiftUp(int index){
        while(index > 1 && data[index/2] < data[index]){
            std::swap(data[index/2], data[index]);
            index = index/2;
        }
    }

    void shiftDown(int index){
        while(index * 2 <= count){
            int j = index * 2; //left child

            if(j + 1 <= count && data[j+1] > data[j]){
                j = j+1;
            }

            if(data[index] > data[j]){
                break;
            }
            std::swap(data[index], data[j]);
            index = j;
        }
    }
};

void heapSort(int arr[], int n){
    MaxHeap<int> heap(n);

    for(int i = 0; i < n; i++){
        heap.add(arr[i]);
    }

    for(int i = n-1; i >= 0; i--){
        arr[i] = heap.extractMax();
    }
}

void heapSort2(int arr[], int n){
    MaxHeap<int> heap(arr, n);

    for(int i = n-1; i >=0; i--){
        arr[i] = heap.extractMax();
    }
}

int main(){
    int n = 10000;
    int arr[n];
    generateRandom(arr, n, 0, n);
    heapSort2(arr, n);
    printArray(arr, n);
 
    return 0;
}