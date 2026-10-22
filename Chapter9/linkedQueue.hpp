#ifndef CHAPTER9_LINKED_QUEUE_HPP
#define CHAPTER9_LINKED_QUEUE_HPP

#include "queue.hpp"
#include "../Chapter6/chainNode.hpp"
#include <limits>
#include <stdexcept>
#include <utility>

// 单链表队列：frontNode_ 指向出队端，backNode_ 指向入队端。
// 空队列时首尾都为 nullptr；非空时尾结点的 next 为 nullptr。
template <class T>
class linkedQueue : public queue<T> {
public:
    linkedQueue() : frontNode_(nullptr), backNode_(nullptr), size_(0) {}
    linkedQueue(const linkedQueue& other)
        : frontNode_(nullptr), backNode_(nullptr), size_(0) {
        try {
            for (chainNode<T>* p = other.frontNode_; p != nullptr; p = p->next)
                push(p->element);
        } catch (...) {
            clear();
            throw;
        }
    }
    ~linkedQueue() override { clear(); }
    linkedQueue& operator=(const linkedQueue& other) {
        linkedQueue copy(other);
        swap(copy);
        return *this;
    }
    void swap(linkedQueue& other) noexcept {
        using std::swap;
        swap(frontNode_, other.frontNode_);
        swap(backNode_, other.backNode_);
        swap(size_, other.size_);
    }
    bool empty() const override { return size_ == 0; }
    int size() const override { return size_; }
    T& front() override {
        if (empty()) throw queueEmpty();
        return frontNode_->element;
    }
    const T& front() const override {
        if (empty()) throw queueEmpty();
        return frontNode_->element;
    }
    T& back() override {
        if (empty()) throw queueEmpty();
        return backNode_->element;
    }
    const T& back() const override {
        if (empty()) throw queueEmpty();
        return backNode_->element;
    }
    void pop() override {
        if (empty()) throw queueEmpty();
        chainNode<T>* old = frontNode_;
        frontNode_ = old->next;
        delete old;
        --size_;
        if (empty()) backNode_ = nullptr;
    }
    void push(const T& value) override {
        if (size_ == std::numeric_limits<int>::max())
            throw std::length_error("queue size overflow");
        chainNode<T>* fresh = new chainNode<T>(value);
        if (backNode_) backNode_->next = fresh;
        else frontNode_ = fresh;
        backNode_ = fresh;
        ++size_;
    }
private:
    void clear() noexcept {
        while (frontNode_) {
            chainNode<T>* next = frontNode_->next;
            delete frontNode_;
            frontNode_ = next;
        }
        backNode_ = nullptr;
        size_ = 0;
    }
    chainNode<T>* frontNode_;
    chainNode<T>* backNode_;
    int size_;
};

#endif
