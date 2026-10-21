#ifndef CHAPTER5_ARRAY_LIST_HPP
#define CHAPTER5_ARRAY_LIST_HPP

#include "iterator.hpp"
#include "linearList.hpp"

#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <utility>

// 教材风格的连续数组线性表。new T[capacity] 会构造所有槽位，
// 因而 T 需要默认构造、复制构造、赋值和相等比较（使用 indexOf 时）。
template <class T>
class arrayList : public linearList<T> {
public:
    explicit arrayList(int initialCapacity = 10)
        : element(nullptr), arrayLength(0), listSize(0) {
        if (initialCapacity < 1)
            throw std::invalid_argument("initialCapacity must be positive");
        element = new T[initialCapacity];
        arrayLength = initialCapacity;
    }

    arrayList(const arrayList& other)
        : element(nullptr), arrayLength(0), listSize(0) {
        std::unique_ptr<T[]> copy(new T[other.arrayLength]);
        for (int i = 0; i < other.listSize; ++i)
            copy[i] = other.element[i];
        element = copy.release();
        arrayLength = other.arrayLength;
        listSize = other.listSize;
    }

    ~arrayList() override { delete[] element; }

    arrayList& operator=(const arrayList& other) {
        arrayList copy(other);
        swap(copy);
        return *this;
    }

    void swap(arrayList& other) noexcept {
        using std::swap;
        swap(element, other.element);
        swap(arrayLength, other.arrayLength);
        swap(listSize, other.listSize);
    }

    bool empty() const override { return listSize == 0; }
    int size() const override { return listSize; }
    int capacity() const { return arrayLength; }

    // 保留 linearList 的教材签名；const 正确的读取请使用 at。
    T& get(int theIndex) const override {
        checkIndex(theIndex);
        return element[theIndex];
    }

    T& at(int theIndex) {
        checkIndex(theIndex);
        return element[theIndex];
    }

    const T& at(int theIndex) const {
        checkIndex(theIndex);
        return element[theIndex];
    }

    int indexOf(const T& theElement) const override {
        for (int i = 0; i < listSize; ++i)
            if (element[i] == theElement) return i;
        return -1;
    }

    void erase(int theIndex) override {
        checkIndex(theIndex);
        for (int i = theIndex; i + 1 < listSize; ++i)
            element[i] = element[i + 1];
        // new T[] 已构造所有槽位；此处不能手动调用析构函数。
        --listSize;
    }

    void insert(int theIndex, const T& theElement) override {
        if (theIndex < 0 || theIndex > listSize)
            throw std::out_of_range("arrayList insert index out of range");

        const T value(theElement); // 允许插入本表中元素的副本。
        if (listSize == arrayLength) {
            if (arrayLength > std::numeric_limits<int>::max() / 2)
                throw std::length_error("arrayList capacity overflow");
            const int newCapacity = 2 * arrayLength;
            std::unique_ptr<T[]> grown(new T[newCapacity]);
            for (int i = 0; i < theIndex; ++i)
                grown[i] = element[i];
            grown[theIndex] = value;
            for (int i = theIndex; i < listSize; ++i)
                grown[i + 1] = element[i];
            delete[] element;
            element = grown.release();
            arrayLength = newCapacity;
        } else {
            for (int i = listSize; i > theIndex; --i)
                element[i] = element[i - 1];
            element[theIndex] = value;
        }
        ++listSize;
    }

    void output(std::ostream& out) const override {
        for (int i = 0; i < listSize; ++i) {
            if (i != 0) out << ' ';
            out << element[i];
        }
    }

    typedef arrayListIterator<T> iterator;
    typedef arrayListIterator<const T> const_iterator;
    iterator begin() { return element; }
    iterator end() { return element + listSize; }
    const_iterator begin() const { return element; }
    const_iterator end() const { return element + listSize; }
    const_iterator cbegin() const { return element; }
    const_iterator cend() const { return element + listSize; }

protected:
    void checkIndex(int theIndex) const {
        if (theIndex < 0 || theIndex >= listSize)
            throw std::out_of_range("arrayList index out of range");
    }

    T* element;
    int arrayLength;
    int listSize;
};

template <class T>
std::ostream& operator<<(std::ostream& out, const arrayList<T>& list) {
    list.output(out);
    return out;
}

#endif
