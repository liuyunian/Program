#include <iostream>

#include <cxxabi.h>     // __cxa_demangle
#include <execinfo.h>   // backtrace backtrace_symbols
#include "Exception.h"

Exception::Exception(const std::string & what) : 
    m_message(what)
{
    fill_stack_trace(/*demangle = */true);
}

void Exception::fill_stack_trace(bool demangle){
    const int max_frames = 200;
    void * frame[max_frames];
    int nptrs = ::backtrace(frame, max_frames);
    char ** strings = ::backtrace_symbols(frame, nptrs); // 字符串数组
    if(strings){
        size_t len = 256;
        char * demangled = demangle ? static_cast<char*>(::malloc(len)) : nullptr;
        for (int i = 1; i < nptrs; ++i){ // skipping the 0-th, which is this function
            if(demangle){
                char * left_par = nullptr;
                char * plus = nullptr;
                for(char* p = strings[i]; *p; ++p){
                    if (*p == '(')
                        left_par = p;
                    else if (*p == '+')
                        plus = p;
                }

                if (left_par && plus){
                    *plus = '\0';
                    int status = 0;
                    char* ret = abi::__cxa_demangle(left_par+1, demangled, &len, &status);
                    *plus = '+';
                    if (status == 0){
                        demangled = ret;  // ret could be realloc()
                        m_stack.append(strings[i], left_par+1);
                        m_stack.append(demangled);
                        m_stack.append(plus);
                        m_stack.push_back('\n');
                        continue;
                    }
                }
            }
            // Fallback to mangled names
            m_stack.append(strings[i]);
            m_stack.push_back('\n');
        }
        free(demangled);
        free(strings);
    }
}