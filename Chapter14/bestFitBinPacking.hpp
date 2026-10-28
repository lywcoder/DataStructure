#ifndef CHAPTER14_BEST_FIT_BIN_PACKING_HPP
#define CHAPTER14_BEST_FIT_BIN_PACKING_HPP

#include "binarySearchTree.hpp"
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

struct BinPackingResult {
    std::vector<int> binOfItem; // 与输入顺序对应，箱号从 0 开始
    std::vector<int> remaining; // 每个箱子的最终剩余容量
};

// 在线 Best-Fit：在所有放得下当前物品的箱中，选择剩余容量最小者；
// 同容量时选择箱号最小者。它是局部最优启发式，不保证全局箱数最少。
inline BinPackingResult bestFitBinPacking(const std::vector<int>& items, int capacity) {
    if (capacity <= 0) throw std::invalid_argument("capacity must be positive");
    for (int size : items)
        if (size <= 0 || size > capacity)
            throw std::invalid_argument("item size must be in (0, capacity]");

    typedef std::pair<int, int> BinKey; // (剩余容量, 箱号)，字典序解决同容量平局
    binarySearchTree<BinKey, bool> available;
    BinPackingResult result;
    result.binOfItem.reserve(items.size());
    for (int size : items) {
        // 找最小的 remaining >= size；箱号从 0 开始，所以 -1 是安全下界。
        const auto* fit = available.lowerBound(BinKey(size, -1));
        int bin;
        if (fit) {
            const BinKey oldKey = fit->first;
            bin = oldKey.second;
            available.erase(oldKey); // 剩余容量改变时，必须先删旧键
        } else {
            if (result.remaining.size() >=
                static_cast<std::size_t>(std::numeric_limits<int>::max()))
                throw std::length_error("too many bins");
            bin = static_cast<int>(result.remaining.size());
            result.remaining.push_back(capacity);
        }
        result.remaining[bin] -= size;
        if (result.remaining[bin] > 0)
            available.insert({BinKey(result.remaining[bin], bin), true});
        result.binOfItem.push_back(bin);
    }
    return result;
}

#endif
