#ifndef linkedQueue_
#define linkedQueue_

#include "queue.hpp"
#include "queueEmpty.hpp"
#include "../Chapter6/chainNode.hpp"

// 用单向链表实现的队列（队尾入队，队首出队）
template<class T>
class linkedQueue : public queue<T> {
public:
    linkedQueue() { queueFront = queueBack = NULL; queueSize = 0; }
    ~linkedQueue();
    bool empty() const { return queueSize == 0; }
    int size() const { return queueSize; }
    T& front();
    T& back();
    void pop();
    void push(const T& theElement);

private:
    chainNode<T>* queueFront;  // 队首结点
    chainNode<T>* queueBack;   // 队尾结点
    int queueSize;             // 元素个数
};

template<class T>
linkedQueue<T>::~linkedQueue() {
    while (queueFront != NULL) {
        chainNode<T>* nextNode = queueFront->next;
        delete queueFront;
        queueFront = nextNode;
    }
}

template<class T>
T& linkedQueue<T>::front() {
    if (queueSize == 0)
        throw queueEmpty();
    return queueFront->element;
}

template<class T>
T& linkedQueue<T>::back() {
    if (queueSize == 0)
        throw queueEmpty();
    return queueBack->element;
}

template<class T>
void linkedQueue<T>::pop() {
    if (queueSize == 0)
        throw queueEmpty();

    chainNode<T>* nextNode = queueFront->next;
    delete queueFront;
    queueFront = nextNode;
    queueSize--;
}

template<class T>
void linkedQueue<T>::push(const T& theElement) {
    chainNode<T>* newNode = new chainNode<T>(theElement, NULL);
    if (queueSize == 0)
        queueFront = newNode;   // 空队列时新结点也是队首
    else
        queueBack->next = newNode;
    queueBack = newNode;
    queueSize++;
}

#endif
