#include <iostream>

void print(std::ostream &out, const char *data){
  out << "data = " << data << std::endl;
}

int main(){
  print(std::cout, "cout");
  print(std::cerr, "cerr");

  return 0;
}