#ifndef CHAPTER6_CIRCULAR_LIST_HPP
#define CHAPTER6_CIRCULAR_LIST_HPP

#include "chainNode.hpp"
#include "extendedLinearList.hpp"

#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

// 带头结点的循环单链表：空表时 headerNode->next == headerNode。
// tailNode 指向最后一个数据结点；空表时 tailNode == headerNode。
template <class T>
class circularListWithHeader : public extendedLinearList<T> {
public:
    circularListWithHeader()
        : headerNode(new chainNode<T>()), tailNode(headerNode), listSize(0) {
        headerNode->next = headerNode;
    }

    circularListWithHeader(const circularListWithHeader& other)
        : headerNode(new chainNode<T>()), tailNode(headerNode), listSize(0) {
        headerNode->next = headerNode;
        try {
            for (chainNode<T>* p = other.headerNode->next;
                 p != other.headerNode; p = p->next)
                push_back(p->element);
        } catch (...) {
            clear();
            delete headerNode;
            throw;
        }
    }

    ~circularListWithHeader() override { clear(); delete headerNode; }

    circularListWithHeader& operator=(const circularListWithHeader& other) {
        circularListWithHeader copy(other);
        swap(copy);
        return *this;
    }

    void swap(circularListWithHeader& other) noexcept {
        using std::swap;
        swap(headerNode, other.headerNode);
        swap(tailNode, other.tailNode);
        swap(listSize, other.listSize);
    }

    bool empty() const override { return listSize == 0; }
    int size() const override { return listSize; }
    T& get(int index) const override { checkIndex(index); return nodeAt(index)->element; }
    T& at(int index) { checkIndex(index); return nodeAt(index)->element; }
    const T& at(int index) const { checkIndex(index); return nodeAt(index)->element; }

    int indexOf(const T& value) const override {
        int index = 0;
        for (chainNode<T>* p = headerNode->next; p != headerNode;
             p = p->next, ++index)
            if (p->element == value) return index;
        return -1;
    }

    void erase(int index) override {
        checkIndex(index);
        chainNode<T>* previous = index == 0 ? headerNode : nodeAt(index - 1);
        chainNode<T>* victim = previous->next;
        previous->next = victim->next;
        if (victim == tailNode) tailNode = previous;
        delete victim;
        --listSize;
    }

    void insert(int index, const T& value) override {
        if (index < 0 || index > listSize)
            throw std::out_of_range("circular list insert index out of range");
        if (index == listSize) { push_back(value); return; }
        chainNode<T>* previous = index == 0 ? headerNode : nodeAt(index - 1);
        previous->next = new chainNode<T>(value, previous->next);
        ++listSize;
    }

    void push_back(const T& value) override {
        if (listSize == std::numeric_limits<int>::max())
            throw std::length_error("circular list size overflow");
        chainNode<T>* fresh = new chainNode<T>(value, headerNode);
        tailNode->next = fresh;
        tailNode = fresh;
        ++listSize;
    }

    void clear() override {
        chainNode<T>* p = headerNode->next;
        while (p != headerNode) {
            chainNode<T>* next = p->next;
            delete p;
            p = next;
        }
        headerNode->next = headerNode;
        tailNode = headerNode;
        listSize = 0;
    }

    void output(std::ostream& out) const override {
        bool first = true;
        for (chainNode<T>* p = headerNode->next; p != headerNode; p = p->next) {
            if (!first) out << ' ';
            out << p->element;
            first = false;
        }
    }

private:
    void checkIndex(int index) const {
        if (index < 0 || index >= listSize)
            throw std::out_of_range("circular list index out of range");
    }
    chainNode<T>* nodeAt(int index) const {
        chainNode<T>* p = headerNode->next;
        for (int i = 0; i < index; ++i) p = p->next;
        return p;
    }

    chainNode<T>* headerNode;
    chainNode<T>* tailNode;
    int listSize;
};

template <class T>
std::ostream& operator<<(std::ostream& out, const circularListWithHeader<T>& list) {
    list.output(out);
    return out;
}

#endif
