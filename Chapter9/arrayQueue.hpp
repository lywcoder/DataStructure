#ifndef arrayQueue_
#define arrayQueue_

#include "queue.hpp"
#include "queueEmpty.hpp"
#include "../Chapter6/illegalParameterValue.hpp"
#include <sstream>

// 用循环数组（环形缓冲区）实现的队列
// 约定：queueFront 指向队首元素的前一个位置，queueBack 指向队尾元素
// 空队列条件：queueFront == queueBack
template<class T>
class arrayQueue : public queue<T> {
public:
    arrayQueue(int initialCapacity = 10);
    ~arrayQueue() { delete [] queue; }

    bool empty() const { return queueFront == queueBack; }
    int size() const { return (queueBack - queueFront + arrayLength) % arrayLength; }
    T& front();
    T& back();
    void pop();
    void push(const T& theElement);

private:
    int queueFront;   // 队首元素的前一个位置
    int queueBack;    // 队尾元素
    int arrayLength;  // 数组容量
    T* queue;         // 存储元素的循环数组
};

template<class T>
arrayQueue<T>::arrayQueue(int initialCapacity) {
    if (initialCapacity < 1) {
        std::ostringstream s;
        s << "Initial capacity = " << initialCapacity << " Must be > 0";
        throw illegalParameterValue(s.str());
    }
    arrayLength = initialCapacity;
    queue = new T[arrayLength];
    queueFront = 0;
    queueBack = 0;
}

template<class T>
T& arrayQueue<T>::front() {
    if (queueFront == queueBack)
        throw queueEmpty();
    return queue[(queueFront + 1) % arrayLength];
}

template<class T>
T& arrayQueue<T>::back() {
    if (queueFront == queueBack)
        throw queueEmpty();
    return queue[queueBack];
}

template<class T>
void arrayQueue<T>::pop() {
    if (queueFront == queueBack)
        throw queueEmpty();

    queueFront = (queueFront + 1) % arrayLength;
    queue[queueFront].~T();   // 析构被删除的元素
}

template<class T>
void arrayQueue<T>::push(const T& theElement) {
    // 队列已满（永远浪费一个存储位置），容量翻倍
    if ((queueBack + 1) % arrayLength == queueFront) {
        T* newQueue = new T[2 * arrayLength];

        // 把现有元素按顺序复制到新数组的前部
        int start = (queueFront + 1) % arrayLength;  // 队首元素的下标
        int numElements = arrayLength - 1;           // 满队列时的元素个数
        for (int i = 0; i < numElements; i++)
            newQueue[i] = queue[(start + i) % arrayLength];

        queueFront = 2 * arrayLength - 1;  // 指向队首的前一个位置
        queueBack = numElements - 1;       // 指向队尾元素
        arrayLength *= 2;

        delete [] queue;
        queue = newQueue;
    }

    queueBack = (queueBack + 1) % arrayLength;
    queue[queueBack] = theElement;
}

#endif
