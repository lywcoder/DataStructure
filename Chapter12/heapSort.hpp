#ifndef CHAPTER12_HEAP_SORT_HPP
#define CHAPTER12_HEAP_SORT_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <vector>

// compare(a,b) 为真表示 a 应排在 b 前；默认升序。
template<class T, class Compare>
void heapSortSiftDown(std::vector<T>& values, std::size_t root,
                      std::size_t heapSize, Compare compare) {
    while (root < heapSize / 2) {
        const std::size_t left = root * 2 + 1;
        std::size_t best = left;
        if (left + 1 < heapSize && compare(values[left], values[left + 1]))
            best = left + 1;
        if (!compare(values[root], values[best])) break;
        std::swap(values[root], values[best]);
        root = best;
    }
}

template<class T, class Compare = std::less<T> >
void heapSort(std::vector<T>& values, Compare compare = Compare()) {
    for (std::size_t i = values.size() / 2; i > 0; --i)
        heapSortSiftDown(values, i - 1, values.size(), compare);
    for (std::size_t end = values.size(); end > 1; --end) {
        std::swap(values[0], values[end - 1]);
        heapSortSiftDown(values, 0, end - 1, compare);
    }
}

#endif
