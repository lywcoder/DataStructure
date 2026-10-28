#ifndef CHAPTER12_MAX_HEAP_HPP
#define CHAPTER12_MAX_HEAP_HPP

#include "pqueue.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <vector>

// 零基数组：父 (i-1)/2，左孩子 2i+1，右孩子 2i+2。
// compare(a,b)==true 表示 a 的优先级低于 b；默认 std::less<T>，即最大堆。
template<class T, class Compare = std::less<T> >
class maxHeap : public maxPriorityQueue<T> {
public:
    explicit maxHeap(const Compare& compare = Compare()) : compare_(compare) {}
    explicit maxHeap(const std::vector<T>& values, const Compare& compare = Compare())
        : data_(values), compare_(compare) {
        if (data_.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("heap size exceeds int range");
        heapify();
    }

    bool empty() const override { return data_.empty(); }
    int size() const override { return static_cast<int>(data_.size()); }
    const T& top() const override {
        if (empty()) throw priorityQueueEmpty();
        return data_.front();
    }
    void push(const T& value) override {
        if (data_.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("heap size exceeds int range");
        data_.push_back(value);
        siftUp(data_.size() - 1);
    }
    void pop() override {
        if (empty()) throw priorityQueueEmpty();
        std::swap(data_.front(), data_.back());
        data_.pop_back();
        if (!empty()) siftDown(0);
    }
    void clear() { data_.clear(); }
    void initialize(const std::vector<T>& values) {
        if (values.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("heap size exceeds int range");
        data_ = values;
        heapify();
    }
    // 与旧草稿的数组入口对应：输入从 values[0] 开始，拷贝数据，不接管所有权。
    void initialize(const T* values, int count) {
        if (count < 0 || (count > 0 && values == nullptr))
            throw std::invalid_argument("invalid heap input");
        if (count == 0) { clear(); return; }
        data_.assign(values, values + count);
        heapify();
    }

private:
    std::vector<T> data_;
    Compare compare_;

    void siftUp(std::size_t child) {
        while (child > 0) {
            const std::size_t parent = (child - 1) / 2;
            if (!compare_(data_[parent], data_[child])) break;
            std::swap(data_[parent], data_[child]);
            child = parent;
        }
    }
    void siftDown(std::size_t parent) {
        const std::size_t n = data_.size();
        while (parent < n / 2) {
            const std::size_t left = parent * 2 + 1;
            std::size_t best = left;
            if (left + 1 < n && compare_(data_[left], data_[left + 1]))
                best = left + 1;
            if (!compare_(data_[parent], data_[best])) break;
            std::swap(data_[parent], data_[best]);
            parent = best;
        }
    }
    void heapify() {
        for (std::size_t i = data_.size() / 2; i > 0; --i)
            siftDown(i - 1);
    }
};

#endif
