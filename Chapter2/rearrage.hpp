#ifndef CHAPTER2_REARRANGE_IN_PLACE_HPP
#define CHAPTER2_REARRANGE_IN_PLACE_HPP

#include <utility>

// 原地重排；r 必须是 0..n-1 的一个排列，调用后也会变为单位排列。
template<class T>
void rearrangeInPlace(T a[], int n, int r[]) {
    for (int i = 0; i < n; i++) {
        while (r[i] != i) {
            int t = r[i];
            std::swap(a[i], a[t]);
            std::swap(r[i], r[t]);
        }
    }
}

#endif
