#ifndef CHAPTER14_HISTOGRAM_HPP
#define CHAPTER14_HISTOGRAM_HPP

#include "binarySearchTree.hpp"
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

// 统计每个不同键出现的次数，并按 Compare 的键序输出。
template<class K, class Compare = std::less<K> >
std::vector<std::pair<K, std::size_t> > histogram(
    const std::vector<K>& data, const Compare& compare = Compare()) {
    binarySearchTree<K, std::size_t, Compare> frequencies(compare);
    for (const K& key : data) {
        typename binarySearchTree<K, std::size_t, Compare>::value_type* entry =
            frequencies.find(key);
        if (entry) ++entry->second;
        else frequencies.insert(std::make_pair(key, std::size_t(1)));
    }
    std::vector<std::pair<K, std::size_t> > result;
    result.reserve(static_cast<std::size_t>(frequencies.size()));
    frequencies.inOrder([&result](const typename binarySearchTree<K, std::size_t, Compare>::value_type& entry) {
        result.emplace_back(entry.first, entry.second);
    });
    return result;
}

#endif
