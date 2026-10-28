#ifndef CHAPTER14_CROSSING_DISTRIBUTION_HPP
#define CHAPTER14_CROSSING_DISTRIBUTION_HPP

#include "indexedBinarySearchTree.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

struct CrossingDistribution {
    std::vector<int> perWire; // 线 i 与其右方的线相交的条数
    std::int64_t total = 0;  // 每对相交线只计一次
};

// 上排端点按 1..n 排列；lowerEnd[i] 是第 i 条线在下排连接的端点。
// 输入须为 1..n 的排列。若 i<j 且 lowerEnd[i]>lowerEnd[j]，两条线相交。
inline CrossingDistribution crossingDistribution(const std::vector<int>& lowerEnd) {
    if (lowerEnd.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::length_error("too many wires");
    const int n = static_cast<int>(lowerEnd.size());
    std::vector<bool> seen(static_cast<std::size_t>(n) + 1, false);
    for (int position : lowerEnd) {
        if (position < 1 || position > n || seen[position])
            throw std::invalid_argument("lower endpoints must be a permutation of 1..n");
        seen[position] = true;
    }

    CrossingDistribution result;
    result.perWire.resize(lowerEnd.size());
    indexedBinarySearchTree<int, int> toTheRight;
    for (int i = n; i > 0; --i) {
        const int endpoint = lowerEnd[static_cast<std::size_t>(i - 1)];
        const int crossings = toTheRight.rank(endpoint); // 右侧已有多少更小端点
        result.perWire[static_cast<std::size_t>(i - 1)] = crossings;
        result.total += crossings;
        toTheRight.insert({endpoint, endpoint});
    }
    return result;
}

#endif
