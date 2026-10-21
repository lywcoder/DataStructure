#ifndef CHAPTER6_RADIX_SORT_HPP
#define CHAPTER6_RADIX_SORT_HPP

#include "chain.hpp"
#include <stdexcept>

// LSD 基数排序：只处理非负整数。每趟按当前位稳定分桶，
// 从最低位到最高位处理后，整个序列递增有序。
inline void radixSort(chain<int>& values, int base = 10) {
    if (base < 2) throw std::invalid_argument("base must be at least 2");
    int largest = 0;
    for (int value : values) {
        if (value < 0) throw std::invalid_argument("radixSort requires nonnegative values");
        if (value > largest) largest = value;
    }

    for (long long place = 1; place <= largest;) {
        values.stableBucketSort(base, [place, base](int value) {
            return static_cast<int>((value / place) % base);
        });
        if (place > largest / base) break; // 避免 place *= base 溢出。
        place *= base;
    }
}

#endif
