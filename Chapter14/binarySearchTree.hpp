#ifndef CHAPTER14_BINARY_SEARCH_TREE_HPP
#define CHAPTER14_BINARY_SEARCH_TREE_HPP

#include "../Chapter10/dictionary.hpp"
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

// 二叉搜索树字典：左子树键 < 根键 < 右子树键。
// Compare 只需满足严格弱序；等价键由 !less(a,b) && !less(b,a) 定义。
template<class K, class E, class Compare = std::less<K> >
class binarySearchTree : public dictionary<K, E> {
public:
    typedef typename dictionary<K, E>::value_type value_type;

    explicit binarySearchTree(const Compare& compare = Compare())
        : root_(nullptr), size_(0), less_(compare) {}
    ~binarySearchTree() override { clear(); }
    binarySearchTree(const binarySearchTree&) = delete;
    binarySearchTree& operator=(const binarySearchTree&) = delete;

    bool empty() const override { return root_ == nullptr; }
    int size() const override { return size_; }

    value_type* find(const K& key) const override {
        Node* current = root_;
        while (current) {
            if (less_(key, current->element.first))
                current = current->left;
            else if (less_(current->element.first, key))
                current = current->right;
            else
                return &current->element; // 等价键：命中
        }
        return nullptr;
    }

    void insert(const value_type& item) override {
        Node** link = &root_; // 始终指向“下一棵子树的入口”
        while (*link) {
            if (less_(item.first, (*link)->element.first))
                link = &(*link)->left;
            else if (less_((*link)->element.first, item.first))
                link = &(*link)->right;
            else {
                (*link)->element.second = item.second; // 同键更新值
                return;
            }
        }
        if (size_ == std::numeric_limits<int>::max())
            throw std::length_error("BST size exceeds int range");
        *link = new Node(item); // 只有找到空入口后才建结点
        ++size_;
    }

    void erase(const K& key) override {
        Node** link = &root_;
        while (*link) {
            if (less_(key, (*link)->element.first))
                link = &(*link)->left;
            else if (less_((*link)->element.first, key))
                link = &(*link)->right;
            else
                break;
        }
        if (!*link) return; // 键不存在，树不变

        Node* target = *link;
        if (!target->left || !target->right) {
            // 0 或 1 个孩子：父入口直接越过待删结点。
            *link = target->left ? target->left : target->right;
        } else {
            // 2 个孩子：找右子树的最小结点（中序后继）。
            Node** successorLink = &target->right;
            while ((*successorLink)->left)
                successorLink = &(*successorLink)->left;
            Node* successor = *successorLink;
            *successorLink = successor->right; // 后继最多只有右孩子，先摘下
            successor->left = target->left;
            successor->right = target->right;
            *link = successor; // 移动结点，不给 const 键赋值
        }
        delete target;
        --size_;
    }

    // 高度按结点层数计算：空树 0，只有根 1。逐层 BFS，O(n) 时间。
    int height() const {
        if (!root_) return 0;
        std::queue<const Node*> pending;
        pending.push(root_);
        int levels = 0;
        while (!pending.empty()) {
            const std::size_t levelSize = pending.size();
            for (std::size_t i = 0; i < levelSize; ++i) {
                const Node* node = pending.front(); pending.pop();
                if (node->left) pending.push(node->left);
                if (node->right) pending.push(node->right);
            }
            ++levels;
        }
        return levels;
    }

    // 中序遍历按键升序访问；visitor 接收 const value_type&。
    template<class Visitor>
    void inOrder(Visitor visitor) const {
        std::vector<const Node*> ancestors;
        const Node* current = root_;
        while (current || !ancestors.empty()) {
            while (current) {
                ancestors.push_back(current);
                current = current->left;
            }
            current = ancestors.back(); ancestors.pop_back();
            visitor(current->element);
            current = current->right;
        }
    }

    void clear() {
        // 逐次把左孩子旋到当前结点上方；没有左孩子时删除当前结点。
        // 无递归栈，也不需要额外分配空间。
        Node* current = root_;
        while (current) {
            if (current->left) {
                Node* left = current->left;
                current->left = left->right;
                left->right = current;
                current = left;
            } else {
                Node* next = current->right;
                delete current;
                current = next;
            }
        }
        root_ = nullptr;
        size_ = 0;
    }

private:
    struct Node {
        value_type element;
        Node* left;
        Node* right;
        explicit Node(const value_type& item)
            : element(item), left(nullptr), right(nullptr) {}
    };
    Node* root_;
    int size_;
    Compare less_;
};

#endif
