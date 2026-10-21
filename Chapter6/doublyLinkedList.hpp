#ifndef CHAPTER6_DOUBLY_LINKED_LIST_HPP
#define CHAPTER6_DOUBLY_LINKED_LIST_HPP

#include "extendedLinearList.hpp"

#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

template <class T>
struct doublyNode {
    T element;
    doublyNode* prev;
    doublyNode* next;

    doublyNode() : element(), prev(nullptr), next(nullptr) {}
    doublyNode(const T& value, doublyNode* before, doublyNode* after)
        : element(value), prev(before), next(after) {}
};

// 循环双链表使用一个头结点：header->next 是头，header->prev 是尾。
// 空表时两个方向都回到 header。
template <class T>
class doublyLinkedList : public extendedLinearList<T> {
public:
    doublyLinkedList() : header(new doublyNode<T>()), listSize(0) {
        header->next = header->prev = header;
    }

    doublyLinkedList(const doublyLinkedList& other)
        : header(new doublyNode<T>()), listSize(0) {
        header->next = header->prev = header;
        try {
            for (doublyNode<T>* p = other.header->next; p != other.header; p = p->next)
                push_back(p->element);
        } catch (...) {
            clear();
            delete header;
            throw;
        }
    }

    ~doublyLinkedList() override { clear(); delete header; }

    doublyLinkedList& operator=(const doublyLinkedList& other) {
        doublyLinkedList copy(other);
        swap(copy);
        return *this;
    }
    void swap(doublyLinkedList& other) noexcept {
        using std::swap;
        swap(header, other.header);
        swap(listSize, other.listSize);
    }

    bool empty() const override { return listSize == 0; }
    int size() const override { return listSize; }
    T& get(int index) const override { checkIndex(index); return nodeAt(index)->element; }
    T& at(int index) { checkIndex(index); return nodeAt(index)->element; }
    const T& at(int index) const { checkIndex(index); return nodeAt(index)->element; }

    T& front() { if (empty()) throw std::out_of_range("empty list"); return header->next->element; }
    T& back() { if (empty()) throw std::out_of_range("empty list"); return header->prev->element; }
    const T& front() const { if (empty()) throw std::out_of_range("empty list"); return header->next->element; }
    const T& back() const { if (empty()) throw std::out_of_range("empty list"); return header->prev->element; }
    const T& secondBack() const {
        if (listSize < 2) throw std::out_of_range("fewer than two elements");
        return header->prev->prev->element;
    }

    int indexOf(const T& value) const override {
        int index = 0;
        for (doublyNode<T>* p = header->next; p != header; p = p->next, ++index)
            if (p->element == value) return index;
        return -1;
    }

    void erase(int index) override {
        checkIndex(index);
        doublyNode<T>* victim = nodeAt(index);
        victim->prev->next = victim->next;
        victim->next->prev = victim->prev;
        delete victim;
        --listSize;
    }

    void insert(int index, const T& value) override {
        if (index < 0 || index > listSize)
            throw std::out_of_range("doubly linked list insert index out of range");
        if (listSize == std::numeric_limits<int>::max())
            throw std::length_error("doubly linked list size overflow");
        doublyNode<T>* after = index == listSize ? header : nodeAt(index);
        doublyNode<T>* fresh = new doublyNode<T>(value, after->prev, after);
        after->prev->next = fresh;
        after->prev = fresh;
        ++listSize;
    }

    void push_back(const T& value) override { insert(listSize, value); }
    void pop_back() { if (empty()) throw std::out_of_range("empty list"); erase(listSize - 1); }

    void clear() override {
        doublyNode<T>* p = header->next;
        while (p != header) {
            doublyNode<T>* next = p->next;
            delete p;
            p = next;
        }
        header->next = header->prev = header;
        listSize = 0;
    }

    void output(std::ostream& out) const override {
        bool first = true;
        for (doublyNode<T>* p = header->next; p != header; p = p->next) {
            if (!first) out << ' ';
            out << p->element;
            first = false;
        }
    }

private:
    void checkIndex(int index) const {
        if (index < 0 || index >= listSize)
            throw std::out_of_range("doubly linked list index out of range");
    }
    doublyNode<T>* nodeAt(int index) const {
        if (index < listSize / 2) {
            doublyNode<T>* p = header->next;
            for (int i = 0; i < index; ++i) p = p->next;
            return p;
        }
        doublyNode<T>* p = header->prev;
        for (int i = listSize - 1; i > index; --i) p = p->prev;
        return p;
    }

    doublyNode<T>* header;
    int listSize;
};

template <class T>
std::ostream& operator<<(std::ostream& out, const doublyLinkedList<T>& list) {
    list.output(out);
    return out;
}

#endif
