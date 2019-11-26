/**
 * runtime_error
*/

#include <iostream>
#include <stdexcept>

void test_func(){
  throw std::runtime_error("error: test_func");
}

int main(){
  try{
    test_func();
  }
  catch(const std::exception& e){
    std::cout << e.what() << std::endl;
  }

  return 0;
}