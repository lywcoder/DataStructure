#ifndef queue_
#define queue_

template<class T>
class queue {
public:
    virtual ~queue() {}
    virtual bool empty() const = 0; //返回true，当前仅当当前队列为空
    virtual int size() const = 0; //返回队列中的元素个数
    virtual T& front() = 0; //返回头元素的引用
    virtual T& back() = 0; //返回尾元素的引用
    virtual void pop() = 0; //删除首元素
    virtual void push(const T& theElement) = 0; //将theElement元素加入队列
};

#endif
