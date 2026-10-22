#ifndef CHAPTER8_ARRAY_STACK_HPP
#define CHAPTER8_ARRAY_STACK_HPP

#include "stack.hpp"
#include <limits>
#include <stdexcept>
#include <vector>

// 用连续数组线性表实现栈：数组尾部就是栈顶。
// std::vector 负责扩容和元素生命期，避免手动析构数组槽位。
template <class T>
class arrayStack : public stack<T> {
public:
    explicit arrayStack(int initialCapacity = 10) {
        if (initialCapacity < 1)
            throw std::invalid_argument("initialCapacity must be positive");
        elements_.reserve(static_cast<std::size_t>(initialCapacity));
    }
    bool empty() const override { return elements_.empty(); }
    int size() const override { return static_cast<int>(elements_.size()); }
    int capacity() const { return static_cast<int>(elements_.capacity()); }
    T& top() override {
        if (empty()) throw stackEmpty();
        return elements_.back();
    }
    const T& top() const override {
        if (empty()) throw stackEmpty();
        return elements_.back();
    }
    void pop() override {
        if (empty()) throw stackEmpty();
        elements_.pop_back();
    }
    void push(const T& value) override {
        if (elements_.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("stack size overflow");
        const T saved(value); // value 可以引用栈中元素；扩容前先保存。
        elements_.push_back(saved);
    }
private:
    std::vector<T> elements_;
};

#endif
