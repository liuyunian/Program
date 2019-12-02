#include <iostream>
#include <fstream>

int main(){
    std::ifstream fin;
    fin.open("test.txt");
    if(fin.is_open()){
        std::string line;
        std::getline(fin, line);
        std::cout << line << std::endl;
    }
    else{
        std::cout << "打开文件失败" << std::endl; 
    }

    fin.close();

    return 0;
}