/**
 * 异常类
 * 对std::exception进一步封装，增加堆栈追踪
*/

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <string>       // string 
#include <exception>    // exception

class Exception : std::exception{
public:
    Exception(const std::string & what);

    ~Exception() noexcept override = default;

    const char * what() const noexcept override {
        return m_message.c_str();
    }

    std::string stack_trace() const noexcept {
        return m_stack.c_str();
    }

private:
    void fill_stack_trace(bool demangle);

private:
    std::string m_message;
    std::string m_stack;
};

#endif // EXCEPTION_H_