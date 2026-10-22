#ifndef CHAPTER8_OFFLINE_EQUIVALENCE_HPP
#define CHAPTER8_OFFLINE_EQUIVALENCE_HPP

#include "linkedstack.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

// 全部等价关系先给出，再一次性求所有等价类（无向图连通分量）。
// DFS 的待处理元素由链栈保存，避免依赖函数递归深度。
inline std::vector<std::vector<int>> offlineEquivalenceClasses(
    int n, const std::vector<std::pair<int, int>>& relations) {
    if (n < 0) throw std::invalid_argument("negative element count");
    std::vector<std::vector<int>> adjacent(static_cast<std::size_t>(n));
    for (const std::pair<int, int>& relation : relations) {
        const int a = relation.first, b = relation.second;
        if (a < 0 || a >= n || b < 0 || b >= n)
            throw std::out_of_range("relation endpoint out of range");
        adjacent[static_cast<std::size_t>(a)].push_back(b);
        adjacent[static_cast<std::size_t>(b)].push_back(a);
    }

    std::vector<char> visited(static_cast<std::size_t>(n), 0);
    std::vector<std::vector<int>> classes;
    for (int start = 0; start < n; ++start) {
        if (visited[static_cast<std::size_t>(start)]) continue;
        std::vector<int> component;
        linkedStack<int> pending;
        pending.push(start);
        visited[static_cast<std::size_t>(start)] = 1;
        while (!pending.empty()) {
            const int current = pending.top();
            pending.pop();
            component.push_back(current);
            for (int next : adjacent[static_cast<std::size_t>(current)])
                if (!visited[static_cast<std::size_t>(next)]) {
                    visited[static_cast<std::size_t>(next)] = 1;
                    pending.push(next);
                }
        }
        std::sort(component.begin(), component.end());
        classes.push_back(component);
    }
    return classes;
}

#endif
