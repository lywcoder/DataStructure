#ifndef CHAPTER11_ARRAY_BINARY_TREE_HPP
#define CHAPTER11_ARRAY_BINARY_TREE_HPP

#include "binaryTree.hpp"
#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <queue>
#include <stdexcept>
#include <vector>

// 位置 0 不使用；i 的左右孩子分别在 2i、2i+1。
// 空槽位是 nullptr，故 T 不必有默认构造函数。
template<class T>
class arrayBinaryTree : public binaryTree<T> {
public:
    typedef typename binaryTree<T>::visitor visitor;
    arrayBinaryTree() : slots_(2), size_(0) {}
    arrayBinaryTree(const arrayBinaryTree&) = delete;
    arrayBinaryTree& operator=(const arrayBinaryTree&) = delete;

    bool empty() const override { return size_ == 0; }
    int size() const override { return size_; }
    int height() const override { return subtreeHeight(1); }
    std::size_t slotCount() const { return slots_.size() - 1; }
    T* at(std::size_t index) {
        return index < slots_.size() ? slots_[index].get() : nullptr;
    }
    const T* at(std::size_t index) const {
        return index < slots_.size() ? slots_[index].get() : nullptr;
    }
    void setRoot(const T& value) {
        if (!empty()) throw std::logic_error("root already exists");
        slots_[1].reset(new T(value));
        ++size_;
    }
    std::size_t insertLeft(std::size_t parent, const T& value) {
        return insertChild(parent, false, value);
    }
    std::size_t insertRight(std::size_t parent, const T& value) {
        return insertChild(parent, true, value);
    }
    void eraseLeft(std::size_t parent) {
        if (!at(parent)) throw std::invalid_argument("parent does not exist");
        eraseSubtree(childIndex(parent, false));
    }
    void eraseRight(std::size_t parent) {
        if (!at(parent)) throw std::invalid_argument("parent does not exist");
        eraseSubtree(childIndex(parent, true));
    }
    void clear() override { slots_.clear(); slots_.resize(2); size_ = 0; }
    void erase() { clear(); }
    void preOrder(const visitor& visit) override { traverse(1, visit, 0); }
    void inOrder(const visitor& visit) override { traverse(1, visit, 1); }
    void postOrder(const visitor& visit) override { traverse(1, visit, 2); }
    void levelOrder(const visitor& visit) override {
        if (empty()) return;
        std::queue<std::size_t> pending;
        pending.push(1);
        while (!pending.empty()) {
            const std::size_t index = pending.front();
            pending.pop();
            visit(*slots_[index]);
            const std::size_t left = index * 2;
            if (at(left)) pending.push(left);
            if (at(left + 1)) pending.push(left + 1);
        }
    }

private:
    std::vector<std::unique_ptr<T> > slots_;
    int size_;
    static std::size_t childIndex(std::size_t parent, bool right) {
        if (parent > (std::numeric_limits<std::size_t>::max() - 1) / 2)
            throw std::length_error("tree index overflow");
        return parent * 2 + (right ? 1 : 0);
    }
    std::size_t insertChild(std::size_t parent, bool right, const T& value) {
        if (!at(parent)) throw std::invalid_argument("parent does not exist");
        const std::size_t index = childIndex(parent, right);
        if (at(index)) throw std::logic_error("child already exists");
        std::unique_ptr<T> element(new T(value));
        if (index >= slots_.size()) slots_.resize(index + 1);
        slots_[index] = std::move(element);
        ++size_;
        return index;
    }
    void eraseSubtree(std::size_t index) {
        if (!at(index)) return;
        eraseSubtree(index * 2);
        eraseSubtree(index * 2 + 1);
        slots_[index].reset();
        --size_;
    }
    int subtreeHeight(std::size_t index) const {
        if (!at(index)) return 0;
        return 1 + std::max(subtreeHeight(index * 2), subtreeHeight(index * 2 + 1));
    }
    void traverse(std::size_t index, const visitor& visit, int order) {
        if (!at(index)) return;
        if (order == 0) visit(*slots_[index]);
        traverse(index * 2, visit, order);
        if (order == 1) visit(*slots_[index]);
        traverse(index * 2 + 1, visit, order);
        if (order == 2) visit(*slots_[index]);
    }
};

#endif
