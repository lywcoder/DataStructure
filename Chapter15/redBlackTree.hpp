#ifndef CHAPTER15_RED_BLACK_TREE_HPP
#define CHAPTER15_RED_BLACK_TREE_HPP

#include "../Chapter10/dictionary.hpp"
#include <algorithm>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>

// 红黑树字典：BST 键序 + 红黑颜色规则。空孩子视为黑色叶子。
template<class K, class E, class Compare = std::less<K> >
class redBlackTree : public dictionary<K, E> {
public:
    typedef typename dictionary<K, E>::value_type value_type;

    explicit redBlackTree(const Compare& compare = Compare())
        : root_(nullptr), size_(0), less_(compare) {}
    ~redBlackTree() override { clear(); }
    redBlackTree(const redBlackTree&) = delete;
    redBlackTree& operator=(const redBlackTree&) = delete;

    bool empty() const override { return root_ == nullptr; }
    int size() const override { return size_; }

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
        Node* parent = nullptr;
        Node* node = root_;
        bool insertLeft = false;
        while (node) {
            parent = node;
            if (less_(item.first, node->element.first)) {
                insertLeft = true;
                node = node->left;
            } else if (less_(node->element.first, item.first)) {
                insertLeft = false;
                node = node->right;
            }
            else { node->element.second = item.second; return; }
        }
        if (size_ == std::numeric_limits<int>::max())
            throw std::length_error("red-black tree size exceeds int range");
        Node* added = new Node(item, parent);
        if (!parent) root_ = added;
        else if (insertLeft) parent->left = added;
        else parent->right = added;
        ++size_;
        fixInsert(added);
    }

    void erase(const K& key) override {
        Node* target = root_;
        while (target) {
            if (less_(key, target->element.first)) target = target->left;
            else if (less_(target->element.first, key)) target = target->right;
            else break;
        }
        if (!target) return;

        Node* removedFromOriginalPlace = target;
        bool removedWasRed = removedFromOriginalPlace->red;
        Node* replacement = nullptr;
        Node* replacementParent = nullptr;
        if (!target->left) {
            replacement = target->right;
            replacementParent = target->parent;
            transplant(target, replacement);
        } else if (!target->right) {
            replacement = target->left;
            replacementParent = target->parent;
            transplant(target, replacement);
        } else {
            // 移动后继结点本身，不能给 pair<const K,E> 的键赋值。
            removedFromOriginalPlace = minimum(target->right);
            removedWasRed = removedFromOriginalPlace->red;
            replacement = removedFromOriginalPlace->right;
            if (removedFromOriginalPlace->parent == target) {
                replacementParent = removedFromOriginalPlace;
            } else {
                replacementParent = removedFromOriginalPlace->parent;
                transplant(removedFromOriginalPlace, replacement);
                removedFromOriginalPlace->right = target->right;
                removedFromOriginalPlace->right->parent = removedFromOriginalPlace;
            }
            transplant(target, removedFromOriginalPlace);
            removedFromOriginalPlace->left = target->left;
            removedFromOriginalPlace->left->parent = removedFromOriginalPlace;
            removedFromOriginalPlace->red = target->red;
        }
        delete target;
        --size_;
        if (!removedWasRed) fixErase(replacement, replacementParent);
    }

    template<class Visitor>
    void inOrder(Visitor visitor) const { inOrderRec(root_, visitor); }
    void clear() { destroy(root_); root_ = nullptr; size_ = 0; }
    int height() const { return nodeHeight(root_); }

    // 教学与测试用：逐结点验证键序、父链接、红结点孩子和相等黑高。
    bool valid() const {
        int blackHeight = 0;
        int count = 0;
        return (!root_ || (!root_->red && root_->parent == nullptr)) &&
               validate(root_, nullptr, nullptr, nullptr, blackHeight, count) &&
               count == size_;
    }

private:
    struct Node {
        value_type element;
        Node* parent;
        Node* left;
        Node* right;
        bool red;
        Node(const value_type& item, Node* p)
            : element(item), parent(p), left(nullptr), right(nullptr), red(true) {}
    };
    Node* root_;
    int size_;
    Compare less_;

    static bool isRed(const Node* node) { return node && node->red; }
    static bool isBlack(const Node* node) { return !isRed(node); }
    static Node* minimum(Node* node) {
        while (node->left) node = node->left;
        return node;
    }

    void rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root_ = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotateRight(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right) y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent) root_ = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        y->right = x;
        x->parent = y;
    }

    void fixInsert(Node* node) {
        while (node->parent && node->parent->red) {
            Node* parent = node->parent;
            Node* grand = parent->parent; // 红父不可能是黑根，祖父必存在
            if (parent == grand->left) {
                Node* uncle = grand->right;
                if (isRed(uncle)) {
                    parent->red = uncle->red = false;
                    grand->red = true;
                    node = grand; // 红色冲突可能向上传播
                } else {
                    if (node == parent->right) {
                        node = parent;
                        rotateLeft(node); // 先把 LR 变成 LL
                    }
                    node->parent->red = false;
                    node->parent->parent->red = true;
                    rotateRight(node->parent->parent);
                }
            } else {
                Node* uncle = grand->left;
                if (isRed(uncle)) {
                    parent->red = uncle->red = false;
                    grand->red = true;
                    node = grand;
                } else {
                    if (node == parent->left) {
                        node = parent;
                        rotateRight(node); // RL 变成 RR
                    }
                    node->parent->red = false;
                    node->parent->parent->red = true;
                    rotateLeft(node->parent->parent);
                }
            }
        }
        root_->red = false;
    }

    void transplant(Node* oldNode, Node* newNode) {
        if (!oldNode->parent) root_ = newNode;
        else if (oldNode == oldNode->parent->left) oldNode->parent->left = newNode;
        else oldNode->parent->right = newNode;
        if (newNode) newNode->parent = oldNode->parent;
    }

    void fixErase(Node* node, Node* parent) {
        // node 可能为空：空叶子仍被视为黑色，parent 指明它原来的位置。
        while (node != root_ && isBlack(node)) {
            if (node == parent->left) {
                Node* sibling = parent->right;
                if (isRed(sibling)) {
                    sibling->red = false;
                    parent->red = true;
                    rotateLeft(parent);
                    sibling = parent->right;
                }
                if (isBlack(sibling ? sibling->left : nullptr) &&
                    isBlack(sibling ? sibling->right : nullptr)) {
                    if (sibling) sibling->red = true;
                    node = parent;
                    parent = node->parent;
                } else {
                    if (isBlack(sibling->right)) {
                        if (sibling->left) sibling->left->red = false;
                        sibling->red = true;
                        rotateRight(sibling);
                        sibling = parent->right;
                    }
                    sibling->red = parent->red;
                    parent->red = false;
                    if (sibling->right) sibling->right->red = false;
                    rotateLeft(parent);
                    node = root_;
                }
            } else {
                Node* sibling = parent->left;
                if (isRed(sibling)) {
                    sibling->red = false;
                    parent->red = true;
                    rotateRight(parent);
                    sibling = parent->left;
                }
                if (isBlack(sibling ? sibling->left : nullptr) &&
                    isBlack(sibling ? sibling->right : nullptr)) {
                    if (sibling) sibling->red = true;
                    node = parent;
                    parent = node->parent;
                } else {
                    if (isBlack(sibling->left)) {
                        if (sibling->right) sibling->right->red = false;
                        sibling->red = true;
                        rotateLeft(sibling);
                        sibling = parent->left;
                    }
                    sibling->red = parent->red;
                    parent->red = false;
                    if (sibling->left) sibling->left->red = false;
                    rotateRight(parent);
                    node = root_;
                }
            }
        }
        if (node) node->red = false;
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
    static int nodeHeight(const Node* node) {
        return node ? 1 + std::max(nodeHeight(node->left), nodeHeight(node->right)) : 0;
    }
    bool validate(const Node* node, const Node* parent, const K* lower,
                  const K* upper, int& blackHeight, int& count) const {
        if (!node) { blackHeight = 1; return true; } // 空叶子计一个黑结点
        const K& key = node->element.first;
        if (node->parent != parent ||
            (lower && !less_(*lower, key)) || (upper && !less_(key, *upper)) ||
            (node->red && (isRed(node->left) || isRed(node->right)))) return false;
        int leftBlack = 0, rightBlack = 0;
        if (!validate(node->left, node, lower, &key, leftBlack, count) ||
            !validate(node->right, node, &key, upper, rightBlack, count) ||
            leftBlack != rightBlack) return false;
        blackHeight = leftBlack + (node->red ? 0 : 1);
        ++count;
        return true;
    }
};

#endif
