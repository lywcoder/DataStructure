#ifndef CHAPTER6_CHAIN_ITERATOR_HPP
#define CHAPTER6_CHAIN_ITERATOR_HPP

#include "chainNode.hpp"
#include <iterator>
#include <type_traits>

// 单链表只能向前走；IsConst 决定解引用后能否修改元素。
template <class T, bool IsConst>
class chainIterator {
public:
    typedef std::forward_iterator_tag iterator_category;
    typedef T value_type;
    typedef std::ptrdiff_t difference_type;
    typedef typename std::conditional<IsConst, const T*, T*>::type pointer;
    typedef typename std::conditional<IsConst, const T&, T&>::type reference;

    explicit chainIterator(chainNode<T>* node = nullptr) : node_(node) {}
    reference operator*() const { return node_->element; }
    pointer operator->() const { return &node_->element; }
    chainIterator& operator++() { node_ = node_->next; return *this; }
    chainIterator operator++(int) { chainIterator old(*this); ++*this; return old; }
    bool operator==(const chainIterator& other) const { return node_ == other.node_; }
    bool operator!=(const chainIterator& other) const { return !(*this == other); }

private:
    chainNode<T>* node_;
};

#endif
