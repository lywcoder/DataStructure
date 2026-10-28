#ifndef CHAPTER11_LINKED_BINARY_TREE_HPP
#define CHAPTER11_LINKED_BINARY_TREE_HPP

#include "binaryTree.hpp"
#include "binaryTreeNode.hpp"
#include "traversal.hpp"
#include <algorithm>
#include <stdexcept>

template<class T>
class linkedBinaryTree : public binaryTree<T> {
public:
    typedef binaryTreeNode<T> node_type;
    typedef typename binaryTree<T>::visitor visitor;

    linkedBinaryTree() : root_(nullptr), size_(0) {}
    ~linkedBinaryTree() override { clear(); }
    linkedBinaryTree(const linkedBinaryTree&) = delete;
    linkedBinaryTree& operator=(const linkedBinaryTree&) = delete;

    bool empty() const override { return size_ == 0; }
    int size() const override { return size_; }
    int height() const override { return subtreeHeight(root_); }
    node_type* rootNode() const { return root_; } // 非拥有指针；树或对应子树删除后失效

    node_type* setRoot(const T& value) {
        if (root_) throw std::logic_error("root already exists");
        root_ = new node_type(value);
        size_ = 1;
        return root_;
    }
    node_type* insertLeft(node_type* parent, const T& value) {
        if (!parent) throw std::invalid_argument("null parent");
        if (parent->leftChild) throw std::logic_error("left child already exists");
        parent->leftChild = new node_type(value);
        ++size_;
        return parent->leftChild;
    }
    node_type* insertRight(node_type* parent, const T& value) {
        if (!parent) throw std::invalid_argument("null parent");
        if (parent->rightChild) throw std::logic_error("right child already exists");
        parent->rightChild = new node_type(value);
        ++size_;
        return parent->rightChild;
    }
    void eraseLeft(node_type* parent) {
        if (!parent) throw std::invalid_argument("null parent");
        destroy(parent->leftChild);
        parent->leftChild = nullptr;
    }
    void eraseRight(node_type* parent) {
        if (!parent) throw std::invalid_argument("null parent");
        destroy(parent->rightChild);
        parent->rightChild = nullptr;
    }
    void clear() override { destroy(root_); root_ = nullptr; }
    void erase() { clear(); } // 保留原有名称

    void preOrder(const visitor& visit) override { ::preOrder(root_, visit); }
    void inOrder(const visitor& visit) override { ::inOrder(root_, visit); }
    void postOrder(const visitor& visit) override { ::postOrder(root_, visit); }
    void levelOrder(const visitor& visit) override { ::levelOrder(root_, visit); }

private:
    node_type* root_;
    int size_;
    static int subtreeHeight(const node_type* node) {
        return node ? 1 + std::max(subtreeHeight(node->leftChild),
                                    subtreeHeight(node->rightChild)) : 0;
    }
    void destroy(node_type* node) {
        if (!node) return;
        destroy(node->leftChild);
        destroy(node->rightChild);
        delete node;
        --size_;
    }
};

#endif
