#ifndef CHAPTER11_TRAVERSAL_HPP
#define CHAPTER11_TRAVERSAL_HPP

#include "binaryTreeNode.hpp"
#include <queue>

// 对不拥有结点的调用者提供遍历；回调接收元素引用。
template<class T, class Visitor>
void preOrder(binaryTreeNode<T>* node, Visitor visit) {
    if (!node) return;
    visit(node->element);
    preOrder(node->leftChild, visit);
    preOrder(node->rightChild, visit);
}

template<class T, class Visitor>
void inOrder(binaryTreeNode<T>* node, Visitor visit) {
    if (!node) return;
    inOrder(node->leftChild, visit);
    visit(node->element);
    inOrder(node->rightChild, visit);
}

template<class T, class Visitor>
void postOrder(binaryTreeNode<T>* node, Visitor visit) {
    if (!node) return;
    postOrder(node->leftChild, visit);
    postOrder(node->rightChild, visit);
    visit(node->element);
}

template<class T, class Visitor>
void levelOrder(binaryTreeNode<T>* root, Visitor visit) {
    if (!root) return;
    std::queue<binaryTreeNode<T>*> pending;
    pending.push(root);
    while (!pending.empty()) {
        binaryTreeNode<T>* node = pending.front();
        pending.pop();
        visit(node->element);
        if (node->leftChild) pending.push(node->leftChild);
        if (node->rightChild) pending.push(node->rightChild);
    }
}

#endif
