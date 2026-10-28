#ifndef CHAPTER14_INDEXED_BINARY_SEARCH_TREE_HPP
#define CHAPTER14_INDEXED_BINARY_SEARCH_TREE_HPP

#include "../Chapter10/dictionary.hpp"
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

// 在普通 BST 上增加子树结点数，使按序号定位和求排名只走一条路径。
// 下标从 0 开始；与普通 BST 一样，这里不自动平衡。
template<class K, class E, class Compare = std::less<K> >
class indexedBinarySearchTree : public dictionary<K, E> {
public:
    typedef typename dictionary<K, E>::value_type value_type;

    explicit indexedBinarySearchTree(const Compare& compare = Compare())
        : root_(nullptr), less_(compare) {}
    ~indexedBinarySearchTree() override { clear(); }
    indexedBinarySearchTree(const indexedBinarySearchTree&) = delete;
    indexedBinarySearchTree& operator=(const indexedBinarySearchTree&) = delete;

    bool empty() const override { return root_ == nullptr; }
    int size() const override { return count(root_); }

    value_type* find(const K& key) const override {
        Node* current = root_;
        while (current) {
            if (less_(key, current->element.first)) current = current->left;
            else if (less_(current->element.first, key)) current = current->right;
            else return &current->element;
        }
        return nullptr;
    }

    // 不存在的下标返回 nullptr；第 i 个即排序后下标 i 的元素。
    value_type* get(int index) const {
        if (index < 0 || index >= size()) return nullptr;
        Node* current = root_;
        while (current) {
            const int leftCount = count(current->left);
            if (index < leftCount) current = current->left;
            else if (index == leftCount) return &current->element;
            else { index -= leftCount + 1; current = current->right; }
        }
        return nullptr;
    }

    // 小于 key 的键有多少个；即 key 的 lower_bound 位置。
    int rank(const K& key) const {
        int result = 0;
        Node* current = root_;
        while (current) {
            if (less_(current->element.first, key)) {
                result += count(current->left) + 1;
                current = current->right;
            } else current = current->left;
        }
        return result;
    }

    // 最小的、不小于 key 的数对；用于最佳适配箱子的容量查找。
    value_type* lowerBound(const K& key) const {
        Node* current = root_;
        Node* candidate = nullptr;
        while (current) {
            if (less_(current->element.first, key)) current = current->right;
            else { candidate = current; current = current->left; }
        }
        return candidate ? &candidate->element : nullptr;
    }

    void insert(const value_type& item) override {
        Node** link = &root_;
        std::vector<Node*> ancestors;
        while (*link) {
            Node* node = *link;
            if (less_(item.first, node->element.first)) {
                ancestors.push_back(node);
                link = &node->left;
            } else if (less_(node->element.first, item.first)) {
                ancestors.push_back(node);
                link = &node->right;
            } else {
                node->element.second = item.second; // 等价键只更新值
                return;
            }
        }
        if (size() == std::numeric_limits<int>::max())
            throw std::length_error("indexed BST size exceeds int range");
        *link = new Node(item);
        for (Node* node : ancestors) ++node->subtreeSize;
    }
    void erase(const K& key) override {
        Node** link = &root_;
        std::vector<Node*> ancestors;
        while (*link) {
            Node* node = *link;
            if (less_(key, node->element.first)) {
                ancestors.push_back(node);
                link = &node->left;
            } else if (less_(node->element.first, key)) {
                ancestors.push_back(node);
                link = &node->right;
            } else break;
        }
        if (*link) removeAtLink(link, ancestors);
    }
    bool eraseAt(int index) {
        if (index < 0 || index >= size()) return false;
        Node** link = &root_;
        std::vector<Node*> ancestors;
        while (*link) {
            Node* node = *link;
            const int leftCount = count(node->left);
            if (index < leftCount) {
                ancestors.push_back(node);
                link = &node->left;
            } else if (index > leftCount) {
                ancestors.push_back(node);
                index -= leftCount + 1;
                link = &node->right;
            } else break;
        }
        removeAtLink(link, ancestors);
        return true;
    }

    int height() const {
        if (!root_) return 0;
        std::queue<const Node*> pending;
        pending.push(root_);
        int levels = 0;
        while (!pending.empty()) {
            const std::size_t width = pending.size();
            for (std::size_t i = 0; i < width; ++i) {
                const Node* node = pending.front(); pending.pop();
                if (node->left) pending.push(node->left);
                if (node->right) pending.push(node->right);
            }
            ++levels;
        }
        return levels;
    }

    template<class Visitor>
    void inOrder(Visitor visit) const {
        std::vector<const Node*> ancestors;
        const Node* current = root_;
        while (current || !ancestors.empty()) {
            while (current) { ancestors.push_back(current); current = current->left; }
            current = ancestors.back(); ancestors.pop_back();
            visit(current->element);
            current = current->right;
        }
    }

    void clear() {
        // 消去左边链接并逐结点释放，避免退化树造成递归析构栈溢出。
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
    }

private:
    struct Node {
        value_type element;
        int subtreeSize;
        Node* left;
        Node* right;
        explicit Node(const value_type& item)
            : element(item), subtreeSize(1), left(nullptr), right(nullptr) {}
    };
    Node* root_;
    Compare less_;

    static int count(const Node* node) { return node ? node->subtreeSize : 0; }
    static void removeAtLink(Node** link, const std::vector<Node*>& ancestors) {
        Node* target = *link;
        if (!target->left || !target->right) {
            *link = target->left ? target->left : target->right;
        } else {
            // 先沿右子树左边找后继；只在后继原位置的祖先上减规模。
            Node** successorLink = &target->right;
            std::vector<Node*> successorAncestors;
            while ((*successorLink)->left) {
                successorAncestors.push_back(*successorLink);
                successorLink = &(*successorLink)->left;
            }
            Node* successor = *successorLink;
            *successorLink = successor->right;
            for (Node* node : successorAncestors) --node->subtreeSize;
            successor->left = target->left;
            successor->right = target->right;
            successor->subtreeSize = target->subtreeSize - 1;
            *link = successor; // 移动结点，不给 pair<const K,E> 的键赋值
        }
        delete target;
        for (Node* node : ancestors) --node->subtreeSize;
    }
};

#endif
