#include <iostream>
#include <execinfo.h>
#include <cxxabi.h>

void test_backtrace(){
  const int maxFrames = 100;                            // 堆栈返回地址的最大个数
  void *frame[maxFrames];                               // 存放堆栈返回地址
  int nptrs = backtrace(frame, maxFrames);
  char **strings = backtrace_symbols(frame, nptrs);     // 字符串数组
  if(strings == nullptr){
    return;
  }

  size_t len = 256;           
  char *demangled = static_cast<char*>(::malloc(len));  // 用于存放demangled之后的结果
  for(int i = 0; i < nptrs; ++ i){
    char *leftPar = nullptr;                            // 左括号
    char *plus = nullptr;                               // 加号
    for(char *p = strings[i]; *p; ++ p){                // 找到左括号和加号的位置，两者之间的内容是需要demangle的
      if(*p == '(')
        leftPar = p;
      else if(*p == '+')
        plus = p;
    }

    *plus = '\0';
    int status = 0;
    char *ret = abi::__cxa_demangle(leftPar+1, demangled, &len, &status);
    *plus = '+';
    if(status != 0){
      std::cout << strings[i] << '\n';
    }
    else{
      demangled = ret;
      std::string stack;
      stack.append(strings[i], leftPar+1);
      stack.append(demangled);
      stack.append(plus);
      std::cout << stack << '\n';
    }
  }

  free(demangled);
  free(strings);
}

int main(){
  test_backtrace();

  return 0;
}