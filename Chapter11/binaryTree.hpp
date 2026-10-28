#ifndef CHAPTER11_BINARY_TREE_HPP
#define CHAPTER11_BINARY_TREE_HPP

#include <functional>

// ADT 规定操作，不规定结点存放在数组还是链表中。
template<class T>
class binaryTree {
public:
    typedef std::function<void(T&)> visitor;
    virtual ~binaryTree() {}
    virtual bool empty() const = 0;
    virtual int size() const = 0;
    virtual int height() const = 0; // 空树高 0，只有根的树高 1
    virtual void preOrder(const visitor& visit) = 0;
    virtual void inOrder(const visitor& visit) = 0;
    virtual void postOrder(const visitor& visit) = 0;
    virtual void levelOrder(const visitor& visit) = 0;
    virtual void clear() = 0;
};

#endif
