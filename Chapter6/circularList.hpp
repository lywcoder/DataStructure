#ifndef circularListWithHeader_
#define circularListWithHeader_

#include "chainNode.hpp"
#include "extendedLinearList.hpp"
#include <iostream>
#include <sstream>

template <class T>
class circularListWithHeader : public extendedLinearList<T> {
public:
    circularListWithHeader();
    circularListWithHeader(const circularListWithHeader<T>&);
    ~circularListWithHeader();

    bool empty() const { return listSize == 0; }
    int size() const { return listSize; }
    T& get(int theIndex) const;
    int indexOf(const T& theElement) const;
    void erase(int theIndex);
    void insert(int theIndex, const T& theElement);
    void output(std::ostream& out) const;

    // extendedLinearList 的方法
    void clear();
    void push_back(const T& theElement);

protected:
    void checkIndex(int theIndex) const;
    chainNode<T>* headerNode;  // 头结点指针
    int listSize;              // 元素个数
};

// 构造函数
template <class T>
circularListWithHeader<T>::circularListWithHeader() {
    headerNode = new chainNode<T>();
    headerNode->next = headerNode;
    listSize = 0;
}

// 拷贝构造函数
template <class T>
circularListWithHeader<T>::circularListWithHeader(const circularListWithHeader<T>& theList) {
    headerNode = new chainNode<T>();
    headerNode->next = headerNode;
    listSize = 0;

    if (theList.listSize == 0)
        return;

    chainNode<T>* sourceNode = theList.headerNode->next;
    chainNode<T>* targetNode = headerNode;
    while (sourceNode != theList.headerNode) {
        targetNode->next = new chainNode<T>(sourceNode->element);
        targetNode = targetNode->next;
        sourceNode = sourceNode->next;
    }
    targetNode->next = headerNode;
    listSize = theList.listSize;
}

// 析构函数
template <class T>
circularListWithHeader<T>::~circularListWithHeader() {
    clear();
    delete headerNode;
}

// 检查下标合法性
template <class T>
void circularListWithHeader<T>::checkIndex(int theIndex) const {
    if (theIndex < 0 || theIndex >= listSize) {
        std::ostringstream s;
        s << "index = " << theIndex << " size = " << listSize;
        throw illegalIndex(s.str());
    }
}

// 返回下标为 theIndex 的元素
template <class T>
T& circularListWithHeader<T>::get(int theIndex) const {
    checkIndex(theIndex);

    chainNode<T>* currentNode = headerNode->next;
    for (int i = 0; i < theIndex; i++)
        currentNode = currentNode->next;

    return currentNode->element;
}

// 返回 theElement 首次出现的下标，不存在返回 -1
template <class T>
int circularListWithHeader<T>::indexOf(const T& theElement) const {
    // 将 theElement 放入头结点作为哨兵
    headerNode->element = theElement;

    chainNode<T>* currentNode = headerNode->next;
    int index = 0;
    while (currentNode->element != theElement) {
        currentNode = currentNode->next;
        index++;
    }

    if (currentNode == headerNode)
        return -1;
    else
        return index;
}

// 删除下标为 theIndex 的元素
template <class T>
void circularListWithHeader<T>::erase(int theIndex) {
    checkIndex(theIndex);

    chainNode<T>* deleteNode;
    if (theIndex == 0) {
        deleteNode = headerNode->next;
        headerNode->next = deleteNode->next;
    } else {
        chainNode<T>* p = headerNode->next;
        for (int i = 0; i < theIndex - 1; i++)
            p = p->next;

        deleteNode = p->next;
        p->next = p->next->next;
    }
    listSize--;
    delete deleteNode;
}

// 在下标 theIndex 处插入 theElement
template <class T>
void circularListWithHeader<T>::insert(int theIndex, const T& theElement) {
    if (theIndex < 0 || theIndex > listSize) {
        std::ostringstream s;
        s << "index = " << theIndex << " size = " << listSize;
        throw illegalIndex(s.str());
    }

    if (theIndex == 0)
        headerNode->next = new chainNode<T>(theElement, headerNode->next);
    else {
        chainNode<T>* p = headerNode->next;
        for (int i = 0; i < theIndex - 1; i++)
            p = p->next;

        p->next = new chainNode<T>(theElement, p->next);
    }
    listSize++;
}

// 输出链表
template <class T>
void circularListWithHeader<T>::output(std::ostream& out) const {
    for (chainNode<T>* currentNode = headerNode->next;
         currentNode != headerNode;
         currentNode = currentNode->next)
        out << currentNode->element << "  ";
}

// 删除除头结点外的所有结点
template <class T>
void circularListWithHeader<T>::clear() {
    while (headerNode->next != headerNode) {
        chainNode<T>* nextNode = headerNode->next->next;
        delete headerNode->next;
        headerNode->next = nextNode;
    }
    listSize = 0;
}

// 在链表尾部插入元素
template <class T>
void circularListWithHeader<T>::push_back(const T& theElement) {
    chainNode<T>* lastNode = headerNode;
    while (lastNode->next != headerNode)
        lastNode = lastNode->next;

    lastNode->next = new chainNode<T>(theElement, headerNode);
    listSize++;
}

// 重载 <<
template <class T>
std::ostream& operator<<(std::ostream& out, const circularListWithHeader<T>& x) {
    x.output(out);
    return out;
}

#endif