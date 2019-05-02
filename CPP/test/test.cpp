#include <iostream>
#include <vector>

int main(){
    std::vector<int> v{0, 1, 2};
    for(std::vector<int>::iterator iter = v.begin(); iter != v.end(); ++ iter){
        v.push_back(3);
        std::cout << *iter << std::endl;
    }

    return 0;
}