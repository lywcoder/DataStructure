#ifndef CHAPTER15_B_TREE_HPP
#define CHAPTER15_B_TREE_HPP

#include "../Chapter10/dictionary.hpp"
#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

// m 阶 B 树：每结点最多 m-1 个键、m 个孩子；所有真实叶子同层。
// 非根结点至少 ceil(m/2)-1 个键。键值对单独分配，移动时不修改 const 键。
template<class K, class E, class Compare = std::less<K> >
class bTree : public dictionary<K, E> {
public:
    typedef typename dictionary<K, E>::value_type value_type;

    explicit bTree(int order, const Compare& compare = Compare())
        : order_(order), root_(), size_(0), less_(compare) {
        if (order < 3) throw std::invalid_argument("B-tree order must be >= 3");
    }
    bTree(const bTree&) = delete;
    bTree& operator=(const bTree&) = delete;

    bool empty() const override { return !root_; }
    int size() const override { return size_; }
    int order() const { return order_; }

    value_type* find(const K& key) const override {
        const Node* node = root_.get();
        while (node) {
            const std::size_t at = position(node, key);
            if (at < node->entries.size() && equivalent(key, node->entries[at]->first))
                return node->entries[at].get();
            node = node->leaf() ? nullptr : node->children[at].get();
        }
        return nullptr;
    }

    value_type* lowerBound(const K& key) const {
        const Node* node = root_.get();
        value_type* candidate = nullptr;
        while (node) {
            const std::size_t at = position(node, key);
            if (at < node->entries.size()) candidate = node->entries[at].get();
            node = node->leaf() ? nullptr : node->children[at].get();
        }
        return candidate;
    }

    void insert(const value_type& item) override {
        value_type* existing = find(item.first);
        if (existing) { existing->second = item.second; return; }
        if (size_ == std::numeric_limits<int>::max())
            throw std::length_error("B-tree size exceeds int range");
        std::unique_ptr<value_type> added(new value_type(item));
        if (!root_) {
            root_.reset(new Node());
            root_->entries.push_back(std::move(added));
        } else {
            Split split;
            insertRec(root_.get(), std::move(added), split);
            if (split.right) {
                std::unique_ptr<Node> newRoot(new Node());
                newRoot->entries.push_back(std::move(split.middle));
                newRoot->children.push_back(std::move(root_));
                newRoot->children.push_back(std::move(split.right));
                root_ = std::move(newRoot);
            }
        }
        ++size_;
    }

    void erase(const K& key) override {
        if (!root_ || !eraseRec(root_.get(), key)) return;
        --size_;
        if (root_->entries.empty()) {
            if (root_->leaf()) root_.reset();
            else {
                std::unique_ptr<Node> onlyChild = std::move(root_->children[0]);
                root_ = std::move(onlyChild); // 旧根被释放，新根高度少一层
            }
        }
    }

    template<class Visitor>
    void inOrder(Visitor visitor) const { inOrderRec(root_.get(), visitor); }
    void clear() { root_.reset(); size_ = 0; }

    // 高度按真实结点层数计算；空树 0。所有叶子同层，只走最左路径。
    int height() const {
        int result = 0;
        const Node* node = root_.get();
        while (node) {
            ++result;
            node = node->leaf() ? nullptr : node->children[0].get();
        }
        return result;
    }

    // 教学与测试用，O(n)：验证键序、结点占用量、孩子数量与叶子深度。
    bool valid() const {
        int leafDepth = -1;
        int count = 0;
        return validate(root_.get(), true, nullptr, nullptr, 1, leafDepth, count) &&
               count == size_;
    }

private:
    struct Node {
        std::vector<std::unique_ptr<value_type> > entries;
        std::vector<std::unique_ptr<Node> > children;
        bool leaf() const { return children.empty(); }
    };
    struct Split {
        std::unique_ptr<value_type> middle;
        std::unique_ptr<Node> right;
    };

    int order_;
    std::unique_ptr<Node> root_;
    int size_;
    Compare less_;

    std::size_t minKeys() const { return static_cast<std::size_t>((order_ - 1) / 2); }
    bool equivalent(const K& a, const K& b) const {
        return !less_(a, b) && !less_(b, a);
    }
    std::size_t position(const Node* node, const K& key) const {
        std::size_t lo = 0, hi = node->entries.size();
        while (lo < hi) {
            const std::size_t mid = lo + (hi - lo) / 2;
            if (less_(node->entries[mid]->first, key)) lo = mid + 1;
            else hi = mid;
        }
        return lo;
    }

    void insertRec(Node* node, std::unique_ptr<value_type> added, Split& out) {
        const std::size_t at = position(node, added->first);
        if (node->leaf()) {
            node->entries.insert(node->entries.begin() + at, std::move(added));
        } else {
            Split childSplit;
            insertRec(node->children[at].get(), std::move(added), childSplit);
            if (childSplit.right) {
                node->entries.insert(node->entries.begin() + at,
                                     std::move(childSplit.middle));
                node->children.insert(node->children.begin() + at + 1,
                                      std::move(childSplit.right));
            }
        }
        if (node->entries.size() == static_cast<std::size_t>(order_))
            splitNode(node, out); // m 个键上溢，中间键升到父结点
    }

    static void splitNode(Node* node, Split& out) {
        const std::size_t mid = node->entries.size() / 2;
        out.right.reset(new Node());
        out.middle = std::move(node->entries[mid]);
        for (std::size_t i = mid + 1; i < node->entries.size(); ++i)
            out.right->entries.push_back(std::move(node->entries[i]));
        node->entries.resize(mid);
        if (!node->leaf()) {
            for (std::size_t i = mid + 1; i < node->children.size(); ++i)
                out.right->children.push_back(std::move(node->children[i]));
            node->children.resize(mid + 1);
        }
    }

    std::unique_ptr<value_type> removeMin(Node* node) {
        if (node->leaf()) {
            std::unique_ptr<value_type> result = std::move(node->entries.front());
            node->entries.erase(node->entries.begin());
            return result;
        }
        std::unique_ptr<value_type> result = removeMin(node->children[0].get());
        if (node->children[0]->entries.size() < minKeys()) repairChild(node, 0);
        return result;
    }

    bool eraseRec(Node* node, const K& key) {
        const std::size_t at = position(node, key);
        if (at < node->entries.size() && equivalent(key, node->entries[at]->first)) {
            if (node->leaf()) node->entries.erase(node->entries.begin() + at);
            else {
                node->entries[at] = removeMin(node->children[at + 1].get());
                if (node->children[at + 1]->entries.size() < minKeys())
                    repairChild(node, at + 1);
            }
            return true;
        }
        if (node->leaf()) return false;
        const bool removed = eraseRec(node->children[at].get(), key);
        if (removed && node->children[at]->entries.size() < minKeys())
            repairChild(node, at);
        return removed;
    }

    void repairChild(Node* parent, std::size_t at) {
        Node* child = parent->children[at].get();
        if (at > 0 && parent->children[at - 1]->entries.size() > minKeys()) {
            Node* left = parent->children[at - 1].get();
            child->entries.insert(child->entries.begin(), std::move(parent->entries[at - 1]));
            parent->entries[at - 1] = std::move(left->entries.back());
            left->entries.pop_back();
            if (!left->leaf()) {
                child->children.insert(child->children.begin(), std::move(left->children.back()));
                left->children.pop_back();
            }
            return;
        }
        if (at + 1 < parent->children.size() &&
            parent->children[at + 1]->entries.size() > minKeys()) {
            Node* right = parent->children[at + 1].get();
            child->entries.push_back(std::move(parent->entries[at]));
            parent->entries[at] = std::move(right->entries.front());
            right->entries.erase(right->entries.begin());
            if (!right->leaf()) {
                child->children.push_back(std::move(right->children.front()));
                right->children.erase(right->children.begin());
            }
            return;
        }
        // 两侧兄弟都不能借：把分隔键降下来，与一个兄弟合并。
        const std::size_t leftAt = at > 0 ? at - 1 : at;
        Node* left = parent->children[leftAt].get();
        Node* right = parent->children[leftAt + 1].get();
        left->entries.push_back(std::move(parent->entries[leftAt]));
        for (auto& entry : right->entries) left->entries.push_back(std::move(entry));
        for (auto& subtree : right->children) left->children.push_back(std::move(subtree));
        parent->entries.erase(parent->entries.begin() + leftAt);
        parent->children.erase(parent->children.begin() + leftAt + 1);
    }

    template<class Visitor>
    static void inOrderRec(const Node* node, Visitor& visit) {
        if (!node) return;
        for (std::size_t i = 0; i < node->entries.size(); ++i) {
            if (!node->leaf()) inOrderRec(node->children[i].get(), visit);
            visit(*node->entries[i]);
        }
        if (!node->leaf()) inOrderRec(node->children.back().get(), visit);
    }

    bool validate(const Node* node, bool root, const K* lower, const K* upper,
                  int depth, int& leafDepth, int& count) const {
        if (!node) return root && size_ == 0;
        if (node->entries.size() > static_cast<std::size_t>(order_ - 1) ||
            node->entries.size() < (root ? 1u : minKeys()) ||
            (!node->leaf() && node->children.size() != node->entries.size() + 1))
            return false;
        for (std::size_t i = 0; i < node->entries.size(); ++i) {
            if (!node->entries[i]) return false;
            const K& key = node->entries[i]->first;
            if ((lower && !less_(*lower, key)) || (upper && !less_(key, *upper)) ||
                (i > 0 && !less_(node->entries[i - 1]->first, key))) return false;
            ++count;
        }
        if (node->leaf()) {
            if (leafDepth < 0) leafDepth = depth;
            return leafDepth == depth;
        }
        for (std::size_t i = 0; i < node->children.size(); ++i) {
            if (!node->children[i]) return false;
            const K* lo = i == 0 ? lower : &node->entries[i - 1]->first;
            const K* hi = i == node->entries.size() ? upper : &node->entries[i]->first;
            if (!validate(node->children[i].get(), false, lo, hi,
                          depth + 1, leafDepth, count)) return false;
        }
        return true;
    }
};

#endif
