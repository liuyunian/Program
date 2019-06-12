#include <iostream>
#include <cassert>

template<typename T>
class MinIndexHeap{
private:
    T * data;
    int * indexes;
    int count;
    int capacity;

public:
    MinIndexHeap(int cap) : data(new T[cap+1]), indexes(new int[cap+1]), count(0), capacity(cap){}

    MinIndexHeap() : data(new T[11]), indexes(new int[11]), count(0), capacity(10){}

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
        indexes[count+1] = count+1;
        count ++;
        shiftUp(count);
    }

    T extractMin(){
        assert(count > 0);

        T elem = data[indexes[1]];
        indexes[1] = indexes[count];
        count --;
        if(count == capacity/4 && capacity/2 >= 10){
            resize(capacity/2);
        }

        shiftDown(1);
        return elem;
    }

    T getElem(int index){
        return data[index+1];
    }

    void printMinIndexHeap(){
        std::cout << "MinIndexHeap: size = " << this->size() << " capacity = " << this->getCapacity() << std::endl;

        std::cout << "[";
        for(int i = 1; i < count; i++){
            std::cout << indexes[i] << " ";
        }
        std::cout << indexes[count] << "]" << std::endl;

        std::cout << "[";
        for(int i = 1; i < count; i++){
            std::cout << data[i] << " ";
        }
        std::cout << data[count] << "]" << std::endl;
    }

private:
    void resize(int newCapacity){
        T * newData = new T[newCapacity];
        int * newIndexes = new int[newCapacity];

        for(int i = 1; i <= count; i++){
            newData[i] = data[i];
            newIndexes[i] = indexes[i];
        }

        data = newData;
        indexes = newIndexes;
        capacity = newCapacity;
    }

    void shiftUp(int index){
        while(index > 1 && data[indexes[index/2]] > data[indexes[index]]){
            std::swap(indexes[index/2], indexes[index]);
            index = index/2;
        }
    }

    void shiftDown(int index){
        while(index*2 <= count){
            int j = index*2;
            if(j+1 <= count && data[indexes[j+1]] < data[indexes[j]]){
                j = j+1;
            }

            if(data[indexes[index]] < data[indexes[j]]){
                break;
            }

            std::swap(indexes[index], indexes[j]);
            index = j;
        }
    }
};

int main(){
    MinIndexHeap<int> mih;
    for(int i = 15 ; i >= 0; i--){
        mih.add(i);
        mih.printMinIndexHeap();
    }

    for(int i = 0; i < 16; i++){
        std::cout << mih.extractMin() << " ";
    }
 
    return 0;
}