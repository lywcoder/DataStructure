#ifndef CHAPTER13_BIN_PACKING_HPP
#define CHAPTER13_BIN_PACKING_HPP

#include "winnerTree.hpp"
#include <functional>
#include <limits>
#include <stdexcept>
#include <vector>

struct PackingResult {
    std::vector<int> binOfItem; // 每件物品的箱号，从 0 开始
    std::vector<int> remaining; // 每个已打开箱子的最终剩余容量
};

inline void validatePackingInput(const std::vector<int>& items, int capacity) {
    if (capacity <= 0) throw std::invalid_argument("capacity must be positive");
    if (items.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::length_error("too many items");
    for (int size : items)
        if (size <= 0 || size > capacity)
            throw std::invalid_argument("item size must be in (0, capacity]");
}

// First-Fit：从左到右找编号最小且放得下物品的箱子。
// 赢者树令“剩余容量较大”者获胜，用子树最大值跳过整段不合格箱子。
inline PackingResult firstFitBinPacking(const std::vector<int>& items, int capacity) {
    validatePackingInput(items, capacity);
    PackingResult result;
    result.binOfItem.reserve(items.size());
    winnerTree<int, std::greater<int>> tree(std::vector<int>(items.size(), 0));
    for (int size : items) {
        int bin = tree.firstIndexAtLeast(size);
        if (bin < 0) {
            bin = static_cast<int>(result.remaining.size());
            result.remaining.push_back(capacity);
        }
        result.remaining[bin] -= size;
        tree.update(bin, result.remaining[bin]); // 沿该箱叶子到根重新组织比赛
        result.binOfItem.push_back(bin);
    }
    return result;
}

// Next-Fit（相邻适配）：只检查当前箱子；放不下就关闭它并新开一个。
inline PackingResult nextFitBinPacking(const std::vector<int>& items, int capacity) {
    validatePackingInput(items, capacity);
    PackingResult result;
    result.binOfItem.reserve(items.size());
    for (int size : items) {
        if (result.remaining.empty() || result.remaining.back() < size)
            result.remaining.push_back(capacity);
        const int bin = static_cast<int>(result.remaining.size()) - 1;
        result.remaining.back() -= size;
        result.binOfItem.push_back(bin);
    }
    return result;
}

#endif
