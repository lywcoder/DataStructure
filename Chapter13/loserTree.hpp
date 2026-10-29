#ifndef CHAPTER13_LOSER_TREE_HPP
#define CHAPTER13_LOSER_TREE_HPP

#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <vector>

// 内部结点只记录该场败者；冠军单独保存在 champion_。
// 支持“替换当前冠军并重赛”，不提供任意非冠军位置的更新。
template <class T, class Compare = std::less<T>>
class loserTree {
public:
    loserTree() : leafCount_(1), champion_(-1) {}

    explicit loserTree(const std::vector<T>& values)
        : leafCount_(1), champion_(-1) { initialize(values); }

    bool empty() const { return values_.empty(); }
    std::size_t size() const { return values_.size(); }

    void initialize(const std::vector<T>& values) {
        if (values.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("too many players");
        values_ = values;
        leafCount_ = 1;
        while (leafCount_ < values_.size()) leafCount_ *= 2;
        losers_.assign(leafCount_, -1);
        std::vector<int> winners(2 * leafCount_, -1);
        for (std::size_t i = 0; i < values_.size(); ++i)
            winners[leafCount_ + i] = static_cast<int>(i);
        // 建树时暂存每棵子树的胜者；正式状态只需各场败者。
        for (std::size_t node = leafCount_ - 1; node > 0; --node) {
            const int a = winners[2 * node], b = winners[2 * node + 1];
            winners[node] = better(a, b);
            losers_[node] = winners[node] == a ? b : a;
        }
        champion_ = winners[1];
    }

    int winnerIndex() const {
        if (empty()) throw std::out_of_range("empty loser tree");
        return champion_;
    }

    const T& winner() const { return values_[winnerIndex()]; }

    void replaceWinner(const T& value) {
        const int replaced = winnerIndex();
        values_[replaced] = value;
        int challenger = replaced;
        // 从原冠军的叶子出发，逐场挑战当初的败者。
        for (std::size_t node = (leafCount_ + static_cast<std::size_t>(replaced)) / 2;
             node > 0; node /= 2) {
            const int opponent = losers_[node];
            const int victor = better(challenger, opponent);
            losers_[node] = victor == challenger ? opponent : challenger;
            challenger = victor;
        }
        champion_ = challenger;
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
    std::vector<int> losers_; // 结点 1..leafCount_-1 为比赛败者
    std::size_t leafCount_;
    int champion_;
    Compare compare_;
};

#endif
