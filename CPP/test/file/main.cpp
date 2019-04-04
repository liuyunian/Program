#include <iostream>
#include <fstream>

// int main(){
//     std::ifstream fin;
//     fin.open("tes.txt");
//     if(fin.is_open()){
//         std::string line;
//         std::getline(fin, line);
//         std::cout << line << std::endl;
//     }
//     else{
//         std::cout << "打开文件失败" << std::endl; 
//     }

//     return 0;
// }

int main(){
    std::ofstream fout;
    fout.open("lyn.txt", std::ios::out | std::ios::app);

    fout <<  "hello world\n";

    return 0;
}