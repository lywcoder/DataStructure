#ifndef chain_
#define chain_

#include <cstddef>
#include <iostream>
#include <sstream>
#include "chainNode.hpp"
#include "../Chapter5/linearList.hpp"


//2. class chain
template <class T>
class chain : public linearList<T> {
public:
    chain(int initialCapacity = 10);
    chain(const chain<T>&);
    ~chain();

    bool empty() const { return listSize == 0; }
    int size() const { return listSize; }
    T& get(int theIndex) const;
    int indexOf(const T& theElement) const;
    void erase(int theIndex);
    void insert(int theIndex, const T& theElement);
    void output(std::ostream& out) const;
    void binSort(int range);

protected:
    void checkIndex(int theIndex) const;
    chainNode<T>* firstNode;
    int listSize;
};

//3. init constructor
// 构造函数和复制构造函数
template<class T>
chain<T>::chain(int initialCapacity) {
    if (initialCapacity < 1) {
        std::ostringstream s;
        s << "Initial capacity = " << initialCapacity << " Must be > 0";
        throw illegalParameterValue(s.str());
    }
    firstNode = NULL;
    listSize = 0;
}

//4. copy constructor
//复制构造函数
template <class T>
chain<T>::chain(const chain<T>& theList) {
    listSize = theList.listSize;

    if (listSize == 0) {
        firstNode = NULL;
        return;
    }

    chainNode<T>* sourceNode = theList.firstNode;
    firstNode = new chainNode<T>(sourceNode->element);
    sourceNode = sourceNode->next;
    chainNode<T>* targetNode = firstNode;
    while (sourceNode != NULL) {
        targetNode->next = new chainNode<T>(sourceNode->element);
        targetNode = targetNode->next;
        sourceNode = sourceNode->next;
    }
    targetNode->next = NULL;
}

//5. destructor
//析构函数
template <class T>
chain<T>::~chain() {
    while (firstNode != NULL) {
        chainNode<T>* nextNode = firstNode->next;
        delete firstNode;
        firstNode = nextNode;
    }
}

//6. checkIndex
//方法checkIndex
template <class T>
void chain<T>::checkIndex(int theIndex) const {
    if (theIndex < 0 || theIndex >= listSize) {
        std::ostringstream s;
        s << "index = " << theIndex << " size = " << listSize;
        throw illegalIndex(s.str());
    }
}

//7. method get
//方法get
template <class T>
T& chain<T>::get(int theIndex) const {
    checkIndex(theIndex);

    chainNode<T>* currentNode = firstNode;
    for (int i = 0; i < theIndex; i++)
        currentNode = currentNode->next;

    return currentNode->element;
}

//7. indexOf
//方法indexOf
template <class T>
int chain<T>::indexOf(const T& theElement) const {
    chainNode<T>* currentNode = firstNode;
    int index = 0;
    while (currentNode != NULL && 
            currentNode->element != theElement) {
                currentNode = currentNode->next;
                index++;
            }
    if (currentNode == NULL) 
            return -1;
    else {
        return index;
    }
}

// 8. erase
// 方法erase
template <class T>
void chain<T>::erase(int theIndex) {
    checkIndex(theIndex);

    chainNode<T>* deleteNode;
    if (theIndex == 0) {
        deleteNode = firstNode;
        firstNode = firstNode->next;
    } else {
        chainNode<T>* p = firstNode;
        for (int i = 0; i < theIndex - 1; i++)
            p = p->next;

        deleteNode = p->next;
        p->next = p->next->next;
    }
    listSize--;
    delete deleteNode;
}

//7. insert
//方法insert
template <class T>
void chain<T>::insert(int theIndex, const T& theElement) {
    if (theIndex < 0 || theIndex > listSize) {
        std::ostringstream s;
        s << "index = " << theIndex << " Size = " << listSize;
        throw illegalIndex(s.str());
    }

    if (theIndex == 0) {
        firstNode = new chainNode<T>(theElement, firstNode);
    } else {
        chainNode<T>* p = firstNode;
        for (int i = 0; i < theIndex - 1; i++) {
            p = p->next;
        }
        p->next = new chainNode<T>(theElement, p->next);
    }
    listSize++;
}

//8. output
//输出方法
template <class T>
void chain<T>::output(std::ostream& out) const {
    for (chainNode<T>* currentNode = firstNode; currentNode != NULL; currentNode = currentNode->next)
        out << currentNode->element << " ";
}

//链表的重载<<
template <class T>
std::ostream& operator<<(std::ostream& out, const chain<T>& x) {
    x.output(out);
    return out;
}

template<class T>
void chain<T>::binSort(int range) {
    //创建并且初始化箱子
    chainNode<T> **bottom, **top;
    bottom = new chainNode<T>* [range + 1];
    top = new chain<T> * [range + 1];
    for (int b = 0; b <= range; b++)
        bottom[b] = NULL;

    //把链表的节点分配给箱子
    for (; firstNode != NULL; firstNode = firstNode->next) {
        //将首节点first node放在箱子中
        int theBin = firstNode->element;
        if (bottom[theBin] == NULL) {
            bottom[theBin] = top[theBin] = firstNode;
        } else {
            top[theBin]->next = firstNode;
            top[theBin] = firstNode;
        }
    }

    //把箱子中的节点收集到有序的列表
    chainNode<T> *y = NULL;
    for (int theBin = 0;  theBin <= range; theBin++) {
        if (bottom[theBin] != NULL) {
            if (y == NULL) 
                firstNode = bottom[theBin];
            else
                y->next = bottom[theBin];
            y = top[theBin];
        }
        if (y != NULL)
            y->next = NULL;
    }
    
    delete[] bottom;
    delete[] top;
}

#endif