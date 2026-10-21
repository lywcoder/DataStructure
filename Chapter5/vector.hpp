#ifndef CHAPTER5_VECTOR_LIST_HPP
#define CHAPTER5_VECTOR_LIST_HPP

#include "linearList.hpp"

#include <algorithm>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <vector>

// 同一线性表接口的标准库实现：由 std::vector 管理连续存储与元素生命期。
template <class T>
class vectorList : public linearList<T> {
public:
    explicit vectorList(int initialCapacity = 10) {
        if (initialCapacity < 1)
            throw std::invalid_argument("initialCapacity must be positive");
        element.reserve(static_cast<std::size_t>(initialCapacity));
    }

    bool empty() const override { return element.empty(); }
    int size() const override { return static_cast<int>(element.size()); }
    int capacity() const { return static_cast<int>(element.capacity()); }

    // 为兼容教材接口保留 T& get(...) const；优先使用 const 正确的 at。
    T& get(int theIndex) const override {
        checkIndex(theIndex);
        return const_cast<T&>(element[static_cast<std::size_t>(theIndex)]);
    }

    T& at(int theIndex) {
        checkIndex(theIndex);
        return element[static_cast<std::size_t>(theIndex)];
    }

    const T& at(int theIndex) const {
        checkIndex(theIndex);
        return element[static_cast<std::size_t>(theIndex)];
    }

    int indexOf(const T& theElement) const override {
        typename std::vector<T>::const_iterator found =
            std::find(element.begin(), element.end(), theElement);
        if (found == element.end()) return -1;
        return static_cast<int>(found - element.begin());
    }

    void erase(int theIndex) override {
        checkIndex(theIndex);
        element.erase(element.begin() + theIndex);
    }

    void insert(int theIndex, const T& theElement) override {
        if (theIndex < 0 || theIndex > size())
            throw std::out_of_range("vectorList insert index out of range");
        if (element.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("vectorList size overflow");
        const T value(theElement); // 即使实参来自本表，扩容也不会使它悬空。
        element.insert(element.begin() + theIndex, value);
    }

    void output(std::ostream& out) const override {
        for (std::size_t i = 0; i < element.size(); ++i) {
            if (i != 0) out << ' ';
            out << element[i];
        }
    }

    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;
    iterator begin() { return element.begin(); }
    iterator end() { return element.end(); }
    const_iterator begin() const { return element.begin(); }
    const_iterator end() const { return element.end(); }
    const_iterator cbegin() const { return element.cbegin(); }
    const_iterator cend() const { return element.cend(); }

private:
    void checkIndex(int theIndex) const {
        if (theIndex < 0 || theIndex >= size())
            throw std::out_of_range("vectorList index out of range");
    }

    std::vector<T> element;
};

template <class T>
std::ostream& operator<<(std::ostream& out, const vectorList<T>& list) {
    list.output(out);
    return out;
}

#endif
