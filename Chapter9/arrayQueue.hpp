#ifndef CHAPTER9_ARRAY_QUEUE_HPP
#define CHAPTER9_ARRAY_QUEUE_HPP

#include "queue.hpp"
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

// 环形数组队列：head_ 是队首槽位，size_ 是有效元素数。
// 逻辑第 k 个元素在 (head_+k)%capacity_，尾部下一空位是 k=size_。
// 用 size_ 区分空和满，因此容量 1 也能正常使用。
template <class T>
class arrayQueue : public queue<T> {
public:
    explicit arrayQueue(int initialCapacity = 10)
        : data_(nullptr), capacity_(0), head_(0), size_(0) {
        if (initialCapacity < 1)
            throw std::invalid_argument("initialCapacity must be positive");
        data_ = allocator_.allocate(static_cast<std::size_t>(initialCapacity));
        capacity_ = initialCapacity;
    }

    arrayQueue(const arrayQueue& other)
        : data_(nullptr), capacity_(0), head_(0), size_(0) {
        data_ = allocator_.allocate(static_cast<std::size_t>(other.capacity_));
        capacity_ = other.capacity_;
        try {
            for (int k = 0; k < other.size_; ++k) {
                allocator_.construct(data_ + k, other.data_[other.slot(k)]);
                ++size_;
            }
        } catch (...) {
            clear();
            allocator_.deallocate(data_, static_cast<std::size_t>(capacity_));
            throw;
        }
    }

    ~arrayQueue() override {
        clear();
        allocator_.deallocate(data_, static_cast<std::size_t>(capacity_));
    }

    arrayQueue& operator=(const arrayQueue& other) {
        arrayQueue copy(other);
        swap(copy);
        return *this;
    }
    void swap(arrayQueue& other) noexcept {
        using std::swap;
        swap(data_, other.data_);
        swap(capacity_, other.capacity_);
        swap(head_, other.head_);
        swap(size_, other.size_);
    }

    bool empty() const override { return size_ == 0; }
    int size() const override { return size_; }
    int capacity() const { return capacity_; }
    T& front() override {
        if (empty()) throw queueEmpty();
        return data_[head_];
    }
    const T& front() const override {
        if (empty()) throw queueEmpty();
        return data_[head_];
    }
    T& back() override {
        if (empty()) throw queueEmpty();
        return data_[slot(size_ - 1)];
    }
    const T& back() const override {
        if (empty()) throw queueEmpty();
        return data_[slot(size_ - 1)];
    }
    void pop() override {
        if (empty()) throw queueEmpty();
        allocator_.destroy(data_ + head_);
        head_ = static_cast<int>((static_cast<std::size_t>(head_) + 1) % capacity_);
        --size_;
        if (empty()) head_ = 0;
    }
    void push(const T& value) override {
        if (size_ == std::numeric_limits<int>::max())
            throw std::length_error("queue size overflow");
        const T saved(value); // 入队值可能引用本队列；扩容前先保存。
        if (size_ == capacity_) grow();
        allocator_.construct(data_ + slot(size_), saved);
        ++size_;
    }

private:
    int slot(int logicalIndex) const {
        return static_cast<int>((static_cast<std::size_t>(head_) +
                                 static_cast<std::size_t>(logicalIndex)) % capacity_);
    }
    void clear() noexcept {
        for (int k = 0; k < size_; ++k) allocator_.destroy(data_ + slot(k));
        size_ = 0;
        head_ = 0;
    }
    void grow() {
        if (capacity_ > std::numeric_limits<int>::max() / 2)
            throw std::length_error("queue capacity overflow");
        const int newCapacity = 2 * capacity_;
        T* fresh = allocator_.allocate(static_cast<std::size_t>(newCapacity));
        int constructed = 0;
        try {
            for (; constructed < size_; ++constructed)
                allocator_.construct(fresh + constructed, data_[slot(constructed)]);
        } catch (...) {
            for (int k = 0; k < constructed; ++k) allocator_.destroy(fresh + k);
            allocator_.deallocate(fresh, static_cast<std::size_t>(newCapacity));
            throw;
        }
        const int oldSize = size_;
        clear();
        allocator_.deallocate(data_, static_cast<std::size_t>(capacity_));
        data_ = fresh;
        capacity_ = newCapacity;
        head_ = 0;
        size_ = oldSize;
    }
    std::allocator<T> allocator_;
    T* data_;
    int capacity_;
    int head_;
    int size_;
};

#endif
