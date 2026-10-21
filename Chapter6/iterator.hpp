#include "chain.hpp"

//链表的迭代器
template <class T>
class iterator {
public:
    iterator(chainNode<T>* theNode = NULL) {
        node = theNode;
    }

    T& operator*() const { return node->element; }
    T* operator->() const { return &node->element; }

    iterator& operator++() {
        node = node->next;
        return *this;
    }
    iterator operator++(int) {
        iterator old = *this;
        node = node->next;
        return old;
    }

    bool operator!=(const iterator right) const {
        return node != right.node;
    }
    
    bool operator==(const iterator right) const {
        return node == right.node;
    }

protected:
    chainNode<T>* node;
};