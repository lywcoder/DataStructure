#ifndef CHAPTER12_PQUEUE_HPP
#define CHAPTER12_PQUEUE_HPP

#include <stdexcept>

class priorityQueueEmpty : public std::out_of_range {
public:
    priorityQueueEmpty() : std::out_of_range("priority queue is empty") {}
};

// 最大优先级队列 ADT：只保证 top() 是当前优先级最高的元素。
template<class T>
class maxPriorityQueue {
public:
    virtual ~maxPriorityQueue() {}
    virtual bool empty() const = 0;
    virtual int size() const = 0;
    virtual const T& top() const = 0;
    virtual void pop() = 0;
    virtual void push(const T& value) = 0;
};

#endif
