#include <iostream>
#include <fstream>

int main(){
    std::ofstream fout;
    fout.open("lyn.txt", std::ios::out | std::ios::app);

    fout <<  "hello world\n";

    return 0;
}