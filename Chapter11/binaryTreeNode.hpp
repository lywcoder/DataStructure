#ifndef CHAPTER11_BINARY_TREE_NODE_HPP
#define CHAPTER11_BINARY_TREE_NODE_HPP

template<class T>
struct binaryTreeNode {
    T element;
    binaryTreeNode* leftChild;
    binaryTreeNode* rightChild;

    explicit binaryTreeNode(const T& value,
                            binaryTreeNode* left = nullptr,
                            binaryTreeNode* right = nullptr)
        : element(value), leftChild(left), rightChild(right) {}
};

#endif
