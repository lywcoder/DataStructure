#include "stack.hpp"
#include "../Chapter5/arrayList.hpp"

// 栈空异常类
class stackEmpty {
public:
    stackEmpty(std::string theMessage = "Stack is empty") : message(theMessage) {}
    void outputMessage() { std::cout << message << std::endl; }
private:
    std::string message;
};

// template<class T>
// class arrayStack : private arrayList<T>, public stack<T> {
// public:
//     arrayStack(int initialCapacity = 10) : arrayList<T>(initialCapacity) {}
//     bool empty() const {
//         return arrayList<T>::empty();
//     }
//     int size() const {
//         return arrayList<T>::size();
//     }
//     T& top() {
//         if (arrayList<T>::empty()) 
//             throw stackEmpty();
//         return get(arrayList<T>::size() - 1);
//     }
//     void pop() {
//         if (arrayList<T>::empty()) 
//             throw stackEmpty();
//         erase(arrayList<T>::size() - 1);
//     }

//     void push(const T& theElement) {
//         insert(arrayList<T>::size(), theElement);
//     }
// };

template<class T>
class arrayStack : public stack<T> {
public:
    arrayStack(int initialCapacity = 10);

    ~arrayStack() { delete[] stack; }
    
    bool empty() const { return stackTop == -1; }

    int size() const { return stackTop + 1; }

    T& top() {
        if (stackTop == -1) 
            throw stackEmpty();
        return stack[stackTop];
    } 

    void pop() {
        if (stackTop == -1) {
            throw stackEmpty();
            stack[stackTop--].~T();
        }
    }

    void push(const T& theElement);
private:
    int stackTop; //当前的栈顶
    int arrayLength; //栈容量
    T* stack; //元素数组
};