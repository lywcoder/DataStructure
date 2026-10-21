#ifndef CHAPTER6_CHAIN_NODE_HPP
#define CHAPTER6_CHAIN_NODE_HPP

// 单链结点：element 保存值，next 指向后继；nullptr 表示链尾。
template <class T>
struct chainNode {
    T element;
    chainNode* next;

    chainNode() : element(), next(nullptr) {}
    explicit chainNode(const T& value, chainNode* nextNode = nullptr)
        : element(value), next(nextNode) {}
};

#endif
