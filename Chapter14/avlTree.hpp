#ifndef CHAPTER14_AVL_TREE_HPP
#define CHAPTER14_AVL_TREE_HPP

#include "../Chapter10/dictionary.hpp"
#include <algorithm>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>

// AVL 字典：BST 键序 + 每个结点的平衡因子属于 {-1,0,1}。
template<class K, class E, class Compare = std::less<K> >
class avlTree : public dictionary<K, E> {
public:
    typedef typename dictionary<K, E>::value_type value_type;

    explicit avlTree(const Compare& compare = Compare())
        : root_(nullptr), size_(0), less_(compare) {}
    ~avlTree() override { clear(); }
    avlTree(const avlTree&) = delete;
    avlTree& operator=(const avlTree&) = delete;

    bool empty() const override { return root_ == nullptr; }
    int size() const override { return size_; }
    int height() const { return nodeHeight(root_); }

    value_type* find(const K& key) const override {
        Node* node = root_;
        while (node) {
            if (less_(key, node->element.first)) node = node->left;
            else if (less_(node->element.first, key)) node = node->right;
            else return &node->element;
        }
        return nullptr;
    }
    value_type* lowerBound(const K& key) const {
        Node* node = root_;
        Node* candidate = nullptr;
        while (node) {
            if (less_(node->element.first, key)) node = node->right;
            else { candidate = node; node = node->left; }
        }
        return candidate ? &candidate->element : nullptr;
    }
    void insert(const value_type& item) override {
        bool added = false;
        root_ = insertRec(root_, item, added);
        if (added) ++size_;
    }
    void erase(const K& key) override {
        bool removed = false;
        root_ = eraseRec(root_, key, removed);
        if (removed) --size_;
    }
    template<class Visitor>
    void inOrder(Visitor visit) const { inOrderRec(root_, visit); }
    void clear() { destroy(root_); root_ = nullptr; size_ = 0; }

    // 教学与测试用：验证键序、记录高度、平衡因子及结点数，O(n)。
    bool valid() const {
        int computedHeight = 0;
        int count = 0;
        return validate(root_, nullptr, nullptr, computedHeight, count) &&
               count == size_;
    }

private:
    struct Node {
        value_type element;
        Node* left;
        Node* right;
        int height;
        explicit Node(const value_type& item)
            : element(item), left(nullptr), right(nullptr), height(1) {}
    };
    Node* root_;
    int size_;
    Compare less_;

    static int nodeHeight(const Node* node) { return node ? node->height : 0; }
    static void updateHeight(Node* node) {
        node->height = 1 + std::max(nodeHeight(node->left), nodeHeight(node->right));
    }
    static int balanceFactor(const Node* node) {
        return nodeHeight(node->left) - nodeHeight(node->right);
    }
    static Node* rotateRight(Node* z) {
        Node* y = z->left;
        z->left = y->right;  // y 与 z 之间的键仍留在 z 的左边
        y->right = z;
        updateHeight(z);     // 先更新下降的结点
        updateHeight(y);
        return y;
    }
    static Node* rotateLeft(Node* z) {
        Node* y = z->right;
        z->right = y->left;
        y->left = z;
        updateHeight(z);
        updateHeight(y);
        return y;
    }
    static Node* rebalance(Node* node) {
        updateHeight(node);
        const int factor = balanceFactor(node);
        if (factor > 1) {
            // LR：左孩子右倾，先转成 LL；删除时孩子 factor=0 也只需单旋。
            if (balanceFactor(node->left) < 0)
                node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        if (factor < -1) {
            // RL：右孩子左倾，先转成 RR。
            if (balanceFactor(node->right) > 0)
                node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        return node;
    }
    Node* insertRec(Node* node, const value_type& item, bool& added) {
        if (!node) {
            if (size_ == std::numeric_limits<int>::max())
                throw std::length_error("AVL size exceeds int range");
            added = true;
            return new Node(item);
        }
        if (less_(item.first, node->element.first))
            node->left = insertRec(node->left, item, added);
        else if (less_(node->element.first, item.first))
            node->right = insertRec(node->right, item, added);
        else { node->element.second = item.second; return node; }
        return added ? rebalance(node) : node;
    }
    static Node* detachMin(Node*& subtree) {
        if (!subtree->left) {
            Node* minimum = subtree;
            subtree = minimum->right;
            return minimum;
        }
        Node* minimum = detachMin(subtree->left);
        subtree = rebalance(subtree);
        return minimum;
    }
    static Node* removeCurrent(Node* node) {
        if (!node->left || !node->right) {
            Node* child = node->left ? node->left : node->right;
            delete node;
            return child;
        }
        Node* successor = detachMin(node->right);
        successor->left = node->left;
        successor->right = node->right;
        delete node; // 移动结点，避免给 pair<const K,E> 的键赋值
        return rebalance(successor);
    }
    Node* eraseRec(Node* node, const K& key, bool& removed) {
        if (!node) return nullptr;
        if (less_(key, node->element.first))
            node->left = eraseRec(node->left, key, removed);
        else if (less_(node->element.first, key))
            node->right = eraseRec(node->right, key, removed);
        else { removed = true; return removeCurrent(node); }
        return removed ? rebalance(node) : node;
    }
    template<class Visitor>
    static void inOrderRec(const Node* node, Visitor& visit) {
        if (!node) return;
        inOrderRec(node->left, visit);
        visit(node->element);
        inOrderRec(node->right, visit);
    }
    static void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
    bool validate(const Node* node, const K* lower, const K* upper,
                  int& computedHeight, int& count) const {
        if (!node) { computedHeight = 0; return true; }
        const K& key = node->element.first;
        if ((lower && !less_(*lower, key)) || (upper && !less_(key, *upper)))
            return false;
        int leftHeight = 0, rightHeight = 0;
        if (!validate(node->left, lower, &key, leftHeight, count) ||
            !validate(node->right, &key, upper, rightHeight, count)) return false;
        computedHeight = 1 + std::max(leftHeight, rightHeight);
        ++count;
        return node->height == computedHeight &&
               leftHeight - rightHeight >= -1 && leftHeight - rightHeight <= 1;
    }
};

#endif
