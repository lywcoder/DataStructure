#ifndef CHAPTER6_DISJOINT_SET_HPP
#define CHAPTER6_DISJOINT_SET_HPP

#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

// 并查集：parent[i] 是 i 的父结点；根结点满足 parent[root] == root。
// 使用路径压缩和按集合大小合并。
class disjointSet {
public:
    explicit disjointSet(int n)
        : parent(checkSize(n)), sizes(static_cast<std::size_t>(n), 1), groups(n) {
        std::iota(parent.begin(), parent.end(), 0);
    }

    int size() const { return static_cast<int>(parent.size()); }
    int components() const { return groups; }

    int find(int x) {
        checkIndex(x);
        int root = x;
        while (root != parent[static_cast<std::size_t>(root)])
            root = parent[static_cast<std::size_t>(root)];
        // 再走一遍，把路径上的结点直接接到根上。
        while (x != root) {
            const int next = parent[static_cast<std::size_t>(x)];
            parent[static_cast<std::size_t>(x)] = root;
            x = next;
        }
        return root;
    }

    bool unite(int a, int b) {
        int rootA = find(a);
        int rootB = find(b);
        if (rootA == rootB) return false;
        if (sizes[static_cast<std::size_t>(rootA)] <
            sizes[static_cast<std::size_t>(rootB)])
            std::swap(rootA, rootB);
        parent[static_cast<std::size_t>(rootB)] = rootA;
        sizes[static_cast<std::size_t>(rootA)] += sizes[static_cast<std::size_t>(rootB)];
        --groups;
        return true;
    }

    bool connected(int a, int b) { return find(a) == find(b); }
    int componentSize(int x) { return sizes[static_cast<std::size_t>(find(x))]; }

private:
    static std::size_t checkSize(int n) {
        if (n < 0) throw std::invalid_argument("disjointSet size must be nonnegative");
        return static_cast<std::size_t>(n);
    }
    void checkIndex(int x) const {
        if (x < 0 || x >= size())
            throw std::out_of_range("disjointSet index out of range");
    }

    std::vector<int> parent;
    std::vector<int> sizes;
    int groups;
};

#endif
