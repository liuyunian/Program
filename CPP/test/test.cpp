#include <iostream>
#include <vector>

int main(){
    std::vector<int> * v_ptr = new std::vector<int>();
    for(int i = 0; i < 10; i++){
        v_ptr->push_back(i);
    }
    
    for(auto item : *v_ptr){
        std::cout << item << std::endl;
    }

    return 0;
}