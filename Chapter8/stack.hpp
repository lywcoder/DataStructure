#ifndef CHAPTER8_STACK_HPP
#define CHAPTER8_STACK_HPP

#include <stdexcept>

// 空栈操作的明确异常类型，调用者可以按需捕获。
class stackEmpty : public std::out_of_range {
public:
    stackEmpty() : std::out_of_range("stack is empty") {}
};

// 栈 ADT：只允许在同一端（栈顶）压入、查看和弹出，遵循 LIFO。
template <class T>
class stack {
public:
    virtual ~stack() {}
    virtual bool empty() const = 0;
    virtual int size() const = 0;
    virtual T& top() = 0;
    virtual const T& top() const = 0;
    virtual void pop() = 0;
    virtual void push(const T& value) = 0;
};

#endif
