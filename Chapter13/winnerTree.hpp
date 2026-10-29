#ifndef CHAPTER13_WINNER_TREE_HPP
#define CHAPTER13_WINNER_TREE_HPP

#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <vector>

// Compare(a, b) 为真表示 a 比 b 更好；默认选最小值。
// 比较结果相等时，固定让下标较小的选手获胜。
template <class T, class Compare = std::less<T>>
class winnerTree {
public:
    winnerTree() : leafCount_(1) {}

    explicit winnerTree(const std::vector<T>& values) : leafCount_(1) {
        initialize(values);
    }

    bool empty() const { return values_.empty(); }
    std::size_t size() const { return values_.size(); }

    void initialize(const std::vector<T>& values) {
        if (values.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("too many players");
        values_ = values; // 树拥有自己的成绩副本
        leafCount_ = 1;
        while (leafCount_ < values_.size()) leafCount_ *= 2;
        tree_.assign(2 * leafCount_, -1); // -1 表示补齐的空位
        for (std::size_t i = 0; i < values_.size(); ++i)
            tree_[leafCount_ + i] = static_cast<int>(i);
        for (std::size_t node = leafCount_ - 1; node > 0; --node)
            tree_[node] = better(tree_[2 * node], tree_[2 * node + 1]);
    }

    int winnerIndex() const {
        if (empty()) throw std::out_of_range("empty winner tree");
        return tree_[1];
    }

    const T& winner() const { return values_[winnerIndex()]; }

    void update(int index, const T& value) {
        if (index < 0 || static_cast<std::size_t>(index) >= values_.size())
            throw std::out_of_range("player index out of range");
        values_[index] = value;
        // 只有这个叶子的祖先比赛可能改变。
        for (std::size_t node = (leafCount_ + static_cast<std::size_t>(index)) / 2;
             node > 0; node /= 2)
            tree_[node] = better(tree_[2 * node], tree_[2 * node + 1]);
    }

    // 仅供“较大值获胜”的树使用：找到最左边 value >= minimum 的叶子。
    // 若子树冠军也小于 minimum，整棵子树都不可能满足条件。
    int firstIndexAtLeast(const T& minimum) const {
        if (empty() || std::less<T>()(values_[tree_[1]], minimum)) return -1;
        std::size_t node = 1;
        while (node < leafCount_) {
            const int left = tree_[2 * node];
            if (left >= 0 && !std::less<T>()(values_[left], minimum))
                node *= 2;
            else
                node = 2 * node + 1;
        }
        return tree_[node];
    }

private:
    int better(int a, int b) const {
        if (a < 0) return b;
        if (b < 0) return a;
        if (compare_(values_[a], values_[b])) return a;
        if (compare_(values_[b], values_[a])) return b;
        return a < b ? a : b;
    }

    std::vector<T> values_;
    std::vector<int> tree_; // 下标 1 为根，叶子从 leafCount_ 开始
    std::size_t leafCount_;
    Compare compare_;
};

#endif
