#ifndef CHAPTER12_MAX_HBLT_HPP
#define CHAPTER12_MAX_HBLT_HPP

#include "pqueue.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

// Height-Biased Leftist Tree（左高树）的最大优先级队列实现。
// s(空)=0；s(x)=从 x 到最近空孩子的最短路径结点数。
// 每个结点满足 s(left)>=s(right)，因此 s(x)=s(right)+1。
template<class T, class Compare = std::less<T> >
class maxHBLT : public maxPriorityQueue<T> {
public:
    explicit maxHBLT(const Compare& compare = Compare())
        : root_(nullptr), size_(0), compare_(compare) {}
    explicit maxHBLT(const std::vector<T>& values, const Compare& compare = Compare())
        : root_(nullptr), size_(0), compare_(compare) { initialize(values); }
    ~maxHBLT() override { clear(); }
    maxHBLT(const maxHBLT&) = delete;
    maxHBLT& operator=(const maxHBLT&) = delete;

    bool empty() const override { return root_ == nullptr; }
    int size() const override { return size_; }
    const T& top() const override {
        if (empty()) throw priorityQueueEmpty();
        return root_->element;
    }
    void push(const T& value) override {
        if (size_ == std::numeric_limits<int>::max())
            throw std::length_error("HBLT size exceeds int range");
        Node* singleton = new Node(value);
        try { root_ = meld(root_, singleton); }
        catch (...) { delete singleton; throw; }
        ++size_;
    }
    void pop() override {
        if (empty()) throw priorityQueueEmpty();
        Node* oldRoot = root_;
        Node* merged = meld(oldRoot->left, oldRoot->right);
        root_ = merged;
        delete oldRoot;
        --size_;
    }

    // 吸收 other 的全部结点；other 变为空。两树须使用相同的优先级规则。
    void merge(maxHBLT& other) {
        if (this == &other || other.empty()) return;
        if (other.size_ > std::numeric_limits<int>::max() - size_)
            throw std::length_error("HBLT size exceeds int range");
        Node* merged = meld(root_, other.root_);
        root_ = merged;
        size_ += other.size_;
        other.root_ = nullptr;
        other.size_ = 0;
    }

    // 两两合并单结点树建树；先建新树，成功后才替换旧树。
    void initialize(const std::vector<T>& values) {
        if (values.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("HBLT size exceeds int range");
        std::queue<std::unique_ptr<maxHBLT> > forest;
        for (const T& value : values) {
            std::unique_ptr<maxHBLT> tree(new maxHBLT(compare_));
            tree->push(value);
            forest.push(std::move(tree));
        }
        while (forest.size() > 1) {
            std::unique_ptr<maxHBLT> first = std::move(forest.front()); forest.pop();
            std::unique_ptr<maxHBLT> second = std::move(forest.front()); forest.pop();
            first->merge(*second);
            forest.push(std::move(first));
        }
        maxHBLT replacement(compare_);
        if (!forest.empty()) replacement.merge(*forest.front());
        std::swap(root_, replacement.root_);
        std::swap(size_, replacement.size_);
    }
    void initialize(const T* values, int count) {
        if (count < 0 || (count > 0 && values == nullptr))
            throw std::invalid_argument("invalid HBLT input");
        if (count == 0) { initialize(std::vector<T>()); return; }
        initialize(std::vector<T>(values, values + count)); // 拷贝，不接管数组
    }
    void clear() {
        destroy(root_);
        root_ = nullptr;
        size_ = 0;
    }

    // 教学/测试用：遍历验证堆序、左高性质、s 值与结点数。O(n)。
    bool valid() const {
        int count = 0;
        return check(root_, count) && count == size_;
    }
    int rootS() const { return rank(root_); }

private:
    struct Node {
        T element;
        int s;
        Node* left;
        Node* right;
        explicit Node(const T& value) : element(value), s(1), left(nullptr), right(nullptr) {}
    };
    Node* root_;
    int size_;
    Compare compare_;

    static int rank(const Node* node) { return node ? node->s : 0; }
    Node* meld(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;
        if (compare_(a->element, b->element)) std::swap(a, b);
        a->right = meld(a->right, b); // 只沿右路径递归
        if (rank(a->left) < rank(a->right)) std::swap(a->left, a->right);
        a->s = rank(a->right) + 1;
        return a;
    }
    static void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
    bool check(const Node* node, int& count) const {
        if (!node) return true;
        ++count;
        if (rank(node->left) < rank(node->right) || node->s != rank(node->right) + 1)
            return false;
        if (node->left && compare_(node->element, node->left->element)) return false;
        if (node->right && compare_(node->element, node->right->element)) return false;
        return check(node->left, count) && check(node->right, count);
    }
};

#endif
