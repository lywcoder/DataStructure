#include "../Chapter6/chainNode.hpp"
#include "stack.hpp"

template<class T>
class linkedStack : public stack<T> {
public:
    linkedStack(int initialCapacity = 10) {
        stackTop = nullptr;
    }

    ~linkedStack();

    bool empty() const {
        return stackSize == 0;
    }

    int size() const {
        return stackSize;
    }

    T& top() {
        if (stackSize == 0) {
            throw stackEmpty();
        }
        return stackTop->element;
    }

    void pop();

    void push(const T& theElement) {
        stackTop = new chainNode<T>(theElement, stackTop);
        stackSize++;
    }
private:
    chainNode<T>* stackTop;
    int stackSize;
};

template<class T>
linkedStack<T>::~linkedStack() {
    //析构函数
    while (stackTop != nullptr) {
        chainNode<T>* nextNode = stackTop->next;
        delete stackTop;
        stackTop = nextNode;
    }
}

template<class T>
void linkedStack<T>::pop() {
    //删除栈顶的节点
    if (stackSize == 0) {
        throw stackEmpty();
    }

    chainNode<T>* nextNode = stackTop->next;
    delete stackTop;
    stackTop = nextNode;
    stackSize--;
}