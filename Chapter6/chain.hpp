#ifndef CHAPTER6_CHAIN_HPP
#define CHAPTER6_CHAIN_HPP

#include "../Chapter5/linearList.hpp"
#include "chainNode.hpp"
#include "iterator.hpp"

#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

// 单链表：结点分散存放，firstNode 指向头，lastNode 指向尾。
template <class T>
class chain : public linearList<T> {
public:
    explicit chain(int initialCapacity = 10)
        : firstNode(nullptr), lastNode(nullptr), listSize(0) {
        // 与教材接口兼容；链表并不预先分配 initialCapacity 个结点。
        if (initialCapacity < 1)
            throw std::invalid_argument("initialCapacity must be positive");
    }

    chain(const chain& other) : firstNode(nullptr), lastNode(nullptr), listSize(0) {
        try {
            for (chainNode<T>* p = other.firstNode; p != nullptr; p = p->next)
                push_back(p->element);
        } catch (...) {
            clear();
            throw;
        }
    }

    ~chain() override { clear(); }

    chain& operator=(const chain& other) {
        chain copy(other);
        swap(copy);
        return *this;
    }

    void swap(chain& other) noexcept {
        using std::swap;
        swap(firstNode, other.firstNode);
        swap(lastNode, other.lastNode);
        swap(listSize, other.listSize);
    }

    bool empty() const override { return listSize == 0; }
    int size() const override { return listSize; }

    // 为兼容 Chapter5 的抽象接口保留 T& get(...) const。
    T& get(int index) const override {
        checkIndex(index);
        return nodeAt(index)->element;
    }
    T& at(int index) { checkIndex(index); return nodeAt(index)->element; }
    const T& at(int index) const { checkIndex(index); return nodeAt(index)->element; }

    int indexOf(const T& value) const override {
        int index = 0;
        for (chainNode<T>* p = firstNode; p != nullptr; p = p->next, ++index)
            if (p->element == value) return index;
        return -1;
    }

    void erase(int index) override {
        checkIndex(index);
        chainNode<T>* victim;
        if (index == 0) {
            victim = firstNode;
            firstNode = victim->next;
            if (listSize == 1) lastNode = nullptr;
        } else {
            chainNode<T>* previous = nodeAt(index - 1);
            victim = previous->next;
            previous->next = victim->next;
            if (victim == lastNode) lastNode = previous;
        }
        delete victim;
        --listSize;
    }

    void insert(int index, const T& value) override {
        if (index < 0 || index > listSize)
            throw std::out_of_range("chain insert index out of range");
        if (index == listSize) { push_back(value); return; }
        if (index == 0) {
            firstNode = new chainNode<T>(value, firstNode);
        } else {
            chainNode<T>* previous = nodeAt(index - 1);
            previous->next = new chainNode<T>(value, previous->next);
        }
        ++listSize;
    }

    void push_back(const T& value) {
        if (listSize == std::numeric_limits<int>::max())
            throw std::length_error("chain size overflow");
        chainNode<T>* fresh = new chainNode<T>(value);
        if (lastNode) lastNode->next = fresh;
        else firstNode = fresh;
        lastNode = fresh;
        ++listSize;
    }

    void clear() noexcept {
        while (firstNode) {
            chainNode<T>* next = firstNode->next;
            delete firstNode;
            firstNode = next;
        }
        lastNode = nullptr;
        listSize = 0;
    }

    void output(std::ostream& out) const override {
        bool first = true;
        for (chainNode<T>* p = firstNode; p != nullptr; p = p->next) {
            if (!first) out << ' ';
            out << p->element;
            first = false;
        }
    }

    typedef chainIterator<T, false> iterator;
    typedef chainIterator<T, true> const_iterator;
    iterator begin() { return iterator(firstNode); }
    iterator end() { return iterator(); }
    const_iterator begin() const { return const_iterator(firstNode); }
    const_iterator end() const { return const_iterator(); }

    // 稳定分桶：先检查全部桶号，随后只重接 next，不复制结点。
    // bucketOf(value) 必须返回 [0, bucketCount) 中的整数。
    template <class Key>
    void stableBucketSort(int bucketCount, Key bucketOf) {
        if (bucketCount <= 0)
            throw std::invalid_argument("bucketCount must be positive");
        std::vector<chainNode<T>*> first(static_cast<std::size_t>(bucketCount), nullptr);
        std::vector<chainNode<T>*> last(static_cast<std::size_t>(bucketCount), nullptr);
        std::vector<int> keys;
        keys.reserve(static_cast<std::size_t>(listSize));
        for (chainNode<T>* p = firstNode; p != nullptr; p = p->next) {
            const int key = bucketOf(p->element);
            if (key < 0 || key >= bucketCount)
                throw std::out_of_range("bucket number out of range");
            keys.push_back(key);
        }

        chainNode<T>* current = firstNode;
        std::size_t position = 0;
        while (current) {
            chainNode<T>* next = current->next;
            current->next = nullptr;
            const int bucket = keys[position++];
            if (last[static_cast<std::size_t>(bucket)])
                last[static_cast<std::size_t>(bucket)]->next = current;
            else
                first[static_cast<std::size_t>(bucket)] = current;
            last[static_cast<std::size_t>(bucket)] = current;
            current = next;
        }

        firstNode = lastNode = nullptr;
        for (int bucket = 0; bucket < bucketCount; ++bucket) {
            chainNode<T>* head = first[static_cast<std::size_t>(bucket)];
            if (!head) continue;
            if (lastNode) lastNode->next = head;
            else firstNode = head;
            lastNode = last[static_cast<std::size_t>(bucket)];
        }
    }

    // 当 T 能转换为 [0, range] 的整数键时，按键稳定排序。
    void binSort(int range) {
        if (range < 0 || range == std::numeric_limits<int>::max())
            throw std::invalid_argument("invalid bucket range");
        stableBucketSort(range + 1, [](const T& value) {
            return static_cast<int>(value);
        });
    }

private:
    void checkIndex(int index) const {
        if (index < 0 || index >= listSize)
            throw std::out_of_range("chain index out of range");
    }
    chainNode<T>* nodeAt(int index) const {
        chainNode<T>* p = firstNode;
        for (int i = 0; i < index; ++i) p = p->next;
        return p;
    }

    chainNode<T>* firstNode;
    chainNode<T>* lastNode;
    int listSize;
};

template <class T>
std::ostream& operator<<(std::ostream& out, const chain<T>& list) {
    list.output(out);
    return out;
}

#endif
