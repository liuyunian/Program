#include <iostream>
#include <cassert>

template <typename T>
class IndexHeap{
private:
    T * data;
    int * indexes;
    int count;
    int capacity;

public:
    IndexHeap(int cap) : data(new T[cap+1]), indexes(new int[cap+1]), count(0), capacity(cap){}

    IndexHeap() : data(new T[11]), indexes(new int[11]), count(0), capacity(10){}

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

    T extractMax(){
        assert(count > 0);

        T elem = data[indexes[1]];
        indexes[1] = indexes[count];
        count --;
        shiftDown(1);

        return elem;
    }

    T getElem(int index){
        return data[index+1];
    }

    void printIndexHeap(){
        std::cout << "IndexHeap: size = " << this->size() << " capacity = " << this->getCapacity() << std::endl;
        
        std::cout << "[";
        for(int i = 0; i < count-1; i++){
            std::cout << indexes[i+1] << " ";
        }
        std::cout << indexes[count] << "]" << std::endl;

        std::cout << "[";
        for(int i = 0; i < count-1; i++){
            std::cout << data[i+1] << " ";
        }
        std::cout << data[count] << "]" << std::endl;
    }

private:
    void resize(int newCapacity){
        T * newData = new T[newCapacity+1];
        int * newIndexes = new int[newCapacity+1];

        for(int i = 1; i<=count; i++){
            newData[i] = data[i];
            newIndexes[i] = indexes[i];
        }

        data = newData;
        indexes = newIndexes;
        capacity = newCapacity;
    }

    void shiftUp(int index){
        while(index > 1 && data[indexes[index]] > data[indexes[index/2]]){
            std::swap(indexes[index], indexes[index/2]);
            index = index/2;
        }
    }

    void shiftDown(int index){
        while(index*2 <= count){
            int j = index*2;
            if(j+1 <= count && data[indexes[j]] < data[indexes[j+1]]){
                j = j+1;
            }

            if(data[indexes[index]] > data[indexes[j]]){
                break;
            }

            std::swap(indexes[index], indexes[j]);
            index = j;
        }
    }
};

int main(){
    IndexHeap<int> ih;
    
    for(int i = 0; i < 16; i++){
        ih.add(i);
        // ih.printIndexHeap();
    }

    for(int i = 0; i < 16; i++){
        std::cout << ih.extractMax() << " ";
    }

    return 0;
}
