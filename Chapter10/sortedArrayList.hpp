#ifndef sortedArrayList_
#define sortedArrayList_

#include "dictionary.hpp"
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <utility>

// 按键升序排列的字典；键相同时更新值，不增加元素个数。
// 数组保存独占指针，移位时不必给 pair<const K, E> 的 const 键赋值。
template<class K, class E>
class sortedArrayList : public dictionary<K, E> {
public:
    typedef std::pair<const K, E> value_type;

    explicit sortedArrayList(int initialCapacity = 10);
    sortedArrayList(const sortedArrayList<K, E>& other);
    sortedArrayList<K, E>& operator=(const sortedArrayList<K, E>& other);
    ~sortedArrayList() override = default;

    bool empty() const override { return listSize == 0; }
    int size() const override { return listSize; }
    int capacity() const { return arrayLength; }
    value_type* find(const K& theKey) const override;
    void erase(const K& theKey) override;
    void insert(const value_type& thePair) override;
    void output(std::ostream& out) const;

private:
    int lowerBound(const K& theKey) const;
    void grow();
    void swap(sortedArrayList<K, E>& other) noexcept;

    int arrayLength;
    int listSize;
    std::unique_ptr<std::unique_ptr<value_type>[]> element;
};

template<class K, class E>
sortedArrayList<K, E>::sortedArrayList(int initialCapacity)
    : arrayLength(initialCapacity), listSize(0) {
    if (initialCapacity < 1)
        throw std::invalid_argument("Initial capacity must be > 0");
    element.reset(new std::unique_ptr<value_type>[arrayLength]);
}

template<class K, class E>
sortedArrayList<K, E>::sortedArrayList(const sortedArrayList<K, E>& other)
    : arrayLength(other.arrayLength), listSize(other.listSize),
      element(new std::unique_ptr<value_type>[arrayLength]) {
    for (int i = 0; i < listSize; ++i)
        element[i].reset(new value_type(*other.element[i]));
}

template<class K, class E>
sortedArrayList<K, E>& sortedArrayList<K, E>::operator=(
    const sortedArrayList<K, E>& other) {
    if (this != &other) {
        sortedArrayList<K, E> copy(other);
        swap(copy);
    }
    return *this;
}

template<class K, class E>
void sortedArrayList<K, E>::swap(sortedArrayList<K, E>& other) noexcept {
    std::swap(arrayLength, other.arrayLength);
    std::swap(listSize, other.listSize);
    element.swap(other.element);
}

// 找到第一个不小于 theKey 的位置；仅要求 K 支持 operator<。
template<class K, class E>
int sortedArrayList<K, E>::lowerBound(const K& theKey) const {
    int left = 0;
    int right = listSize;
    while (left < right) {
        int middle = left + (right - left) / 2;
        if (element[middle]->first < theKey)
            left = middle + 1;
        else
            right = middle;
    }
    return left;
}

template<class K, class E>
typename sortedArrayList<K, E>::value_type*
sortedArrayList<K, E>::find(const K& theKey) const {
    int index = lowerBound(theKey);
    if (index < listSize && !(theKey < element[index]->first))
        return element[index].get();
    return nullptr;
}

template<class K, class E>
void sortedArrayList<K, E>::grow() {
    if (arrayLength > std::numeric_limits<int>::max() / 2)
        throw std::length_error("sortedArrayList capacity is too large");

    int newLength = arrayLength * 2;
    std::unique_ptr<std::unique_ptr<value_type>[]> newElement(
        new std::unique_ptr<value_type>[newLength]);
    for (int i = 0; i < listSize; ++i)
        newElement[i] = std::move(element[i]);
    element.swap(newElement);
    arrayLength = newLength;
}

template<class K, class E>
void sortedArrayList<K, E>::insert(const value_type& thePair) {
    int index = lowerBound(thePair.first);
    if (index < listSize && !(thePair.first < element[index]->first)) {
        element[index]->second = thePair.second;
        return;
    }

    // 先复制数对，避免分配或复制失败时已移动了原数组中的元素。
    std::unique_ptr<value_type> newPair(new value_type(thePair));
    if (listSize == arrayLength)
        grow();
    for (int i = listSize; i > index; --i)
        element[i] = std::move(element[i - 1]);
    element[index] = std::move(newPair);
    ++listSize;
}

template<class K, class E>
void sortedArrayList<K, E>::erase(const K& theKey) {
    int index = lowerBound(theKey);
    if (index == listSize || theKey < element[index]->first)
        return;

    element[index].reset();
    for (int i = index; i < listSize - 1; ++i)
        element[i] = std::move(element[i + 1]);
    element[--listSize].reset();
}

template<class K, class E>
void sortedArrayList<K, E>::output(std::ostream& out) const {
    for (int i = 0; i < listSize; ++i)
        out << element[i]->first << ' ' << element[i]->second << '\n';
}

template<class K, class E>
std::ostream& operator<<(std::ostream& out, const sortedArrayList<K, E>& x) {
    x.output(out);
    return out;
}

#endif
