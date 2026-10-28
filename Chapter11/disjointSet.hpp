#ifndef CHAPTER11_DISJOINT_SET_HPP
#define CHAPTER11_DISJOINT_SET_HPP

#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

// 每个集合是一棵以代表元为根的普通树；parent_[root] == root。
class disjointSet {
public:
    explicit disjointSet(int count)
        : parent_(count >= 0 ? static_cast<std::size_t>(count) : 0),
          sizes_(parent_.size(), 1), components_(count) {
        if (count < 0) throw std::invalid_argument("negative element count");
        std::iota(parent_.begin(), parent_.end(), 0);
    }
    int count() const { return components_; }
    int find(int element) {
        check(element);
        if (parent_[element] != element)
            parent_[element] = find(parent_[element]); // 路径压缩
        return parent_[element];
    }
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (sizes_[a] < sizes_[b]) std::swap(a, b);
        parent_[b] = a; // 小树挂到大树根上
        sizes_[a] += sizes_[b];
        --components_;
        return true;
    }
    bool connected(int a, int b) { return find(a) == find(b); }
    int componentSize(int element) { return sizes_[find(element)]; }
private:
    std::vector<int> parent_;
    std::vector<int> sizes_;
    int components_;
    void check(int element) const {
        if (element < 0 || static_cast<std::size_t>(element) >= parent_.size())
            throw std::out_of_range("element index out of range");
    }
};

#endif
