/**
 * 异常中最基础的内容
 * 抛出异常：throw
 * 异常捕获：try - catch
*/

#include <iostream>

void test_func(int errCode){
  throw errCode;
}

int divide(int a, int b){
  if(b == 0){
    throw "error: can't divide by 0";
  }

  // std::cout << 1 << std::endl;
  return a/b;
}

int main(){
  try{
    test_func(2);
  }
  catch(int errCode){
    std::cout << "Exception: errCode = " << errCode << std::endl;
  }

  int result = 0;
  try{
    result = divide(10, 0);
  }
  catch(const char *err){
    std::cout << "Exception: " << err << std::endl;
  }

  std::cout << result << std::endl;

  return 0;
}