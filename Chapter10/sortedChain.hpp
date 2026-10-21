#ifndef sortedChain_
#define sortedChain_

#include "dictionary.hpp"
#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

// 按键升序排列的单链表字典。
template<class K, class E>
class sortedChain : public dictionary<K, E> {
public:
    typedef std::pair<const K, E> value_type;

    sortedChain() : firstNode(nullptr), listSize(0) {}
    sortedChain(const sortedChain<K, E>& other);
    sortedChain<K, E>& operator=(const sortedChain<K, E>& other);
    ~sortedChain() override;

    bool empty() const override { return listSize == 0; }
    int size() const override { return listSize; }
    value_type* find(const K& theKey) const override;
    void erase(const K& theKey) override;
    void insert(const value_type& thePair) override;
    void output(std::ostream& out) const;

private:
    // 直接构造 element，支持 const 键及没有默认构造函数的类型。
    struct node {
        value_type element;
        node* next;
        node(const value_type& thePair, node* nextNode = nullptr)
            : element(thePair), next(nextNode) {}
    };

    void swap(sortedChain<K, E>& other) noexcept;

    node* firstNode;
    int listSize;
};

template<class K, class E>
sortedChain<K, E>::sortedChain(const sortedChain<K, E>& other)
    : firstNode(nullptr), listSize(0) {
    // 临时链表负责清理已复制结点，即使中途复制失败也不会泄漏。
    sortedChain<K, E> copy;
    node** tail = &copy.firstNode;
    for (node* current = other.firstNode; current != nullptr;
         current = current->next) {
        *tail = new node(current->element);
        tail = &(*tail)->next;
        ++copy.listSize;
    }
    swap(copy);
}

template<class K, class E>
sortedChain<K, E>& sortedChain<K, E>::operator=(
    const sortedChain<K, E>& other) {
    if (this != &other) {
        sortedChain<K, E> copy(other);
        swap(copy);
    }
    return *this;
}

template<class K, class E>
void sortedChain<K, E>::swap(sortedChain<K, E>& other) noexcept {
    std::swap(firstNode, other.firstNode);
    std::swap(listSize, other.listSize);
}

template<class K, class E>
sortedChain<K, E>::~sortedChain() {
    while (firstNode != nullptr) {
        node* nextNode = firstNode->next;
        delete firstNode;
        firstNode = nextNode;
    }
}

template<class K, class E>
typename sortedChain<K, E>::value_type*
sortedChain<K, E>::find(const K& theKey) const {
    node* current = firstNode;
    while (current != nullptr && current->element.first < theKey)
        current = current->next;
    if (current != nullptr && !(theKey < current->element.first))
        return &current->element;
    return nullptr;
}

template<class K, class E>
void sortedChain<K, E>::insert(const value_type& thePair) {
    node* previous = nullptr;
    node* current = firstNode;
    while (current != nullptr && current->element.first < thePair.first) {
        previous = current;
        current = current->next;
    }

    if (current != nullptr && !(thePair.first < current->element.first)) {
        current->element.second = thePair.second;
        return;
    }
    if (listSize == std::numeric_limits<int>::max())
        throw std::length_error("sortedChain size is too large");

    node* newNode = new node(thePair, current);
    if (previous == nullptr)
        firstNode = newNode;
    else
        previous->next = newNode;
    ++listSize;
}

template<class K, class E>
void sortedChain<K, E>::erase(const K& theKey) {
    node* previous = nullptr;
    node* current = firstNode;
    while (current != nullptr && current->element.first < theKey) {
        previous = current;
        current = current->next;
    }
    if (current == nullptr || theKey < current->element.first)
        return;

    if (previous == nullptr)
        firstNode = current->next;
    else
        previous->next = current->next;
    delete current;
    --listSize;
}

template<class K, class E>
void sortedChain<K, E>::output(std::ostream& out) const {
    for (node* current = firstNode; current != nullptr; current = current->next)
        out << current->element.first << ' ' << current->element.second << '\n';
}

template<class K, class E>
std::ostream& operator<<(std::ostream& out, const sortedChain<K, E>& x) {
    x.output(out);
    return out;
}

#endif
