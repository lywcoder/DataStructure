#ifndef CHAPTER8_LINKED_STACK_HPP
#define CHAPTER8_LINKED_STACK_HPP

#include "stack.hpp"
#include "../Chapter6/chainNode.hpp"
#include <limits>
#include <stdexcept>
#include <utility>

// 用单链表描述栈：头结点就是栈顶；压入和弹出都只改头链接。
template <class T>
class linkedStack : public stack<T> {
public:
    explicit linkedStack(int initialCapacity = 10) : topNode_(nullptr), size_(0) {
        // 链栈不预留容量；参数只为与数组栈构造形式一致。
        if (initialCapacity < 1)
            throw std::invalid_argument("initialCapacity must be positive");
    }
    linkedStack(const linkedStack& other) : topNode_(nullptr), size_(0) {
        chainNode<T>* tail = nullptr;
        try {
            for (chainNode<T>* p = other.topNode_; p != nullptr; p = p->next) {
                chainNode<T>* fresh = new chainNode<T>(p->element);
                if (tail) tail->next = fresh;
                else topNode_ = fresh;
                tail = fresh;
                ++size_;
            }
        } catch (...) {
            clear();
            throw;
        }
    }
    ~linkedStack() override { clear(); }
    linkedStack& operator=(const linkedStack& other) {
        linkedStack copy(other);
        swap(copy);
        return *this;
    }
    void swap(linkedStack& other) noexcept {
        using std::swap;
        swap(topNode_, other.topNode_);
        swap(size_, other.size_);
    }
    bool empty() const override { return size_ == 0; }
    int size() const override { return size_; }
    T& top() override {
        if (empty()) throw stackEmpty();
        return topNode_->element;
    }
    const T& top() const override {
        if (empty()) throw stackEmpty();
        return topNode_->element;
    }
    void pop() override {
        if (empty()) throw stackEmpty();
        chainNode<T>* old = topNode_;
        topNode_ = old->next;
        delete old;
        --size_;
    }
    void push(const T& value) override {
        if (size_ == std::numeric_limits<int>::max())
            throw std::length_error("stack size overflow");
        topNode_ = new chainNode<T>(value, topNode_);
        ++size_;
    }
private:
    void clear() noexcept {
        while (topNode_) {
            chainNode<T>* next = topNode_->next;
            delete topNode_;
            topNode_ = next;
        }
        size_ = 0;
    }
    chainNode<T>* topNode_;
    int size_;
};

#endif
