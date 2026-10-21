#ifndef CHAPTER3_BINARY_HPP
#define CHAPTER3_BINARY_HPP

// 前提：n >= 0；n > 0 时 a 指向至少 n 个按非递减顺序排列的元素。
// 返回找到的某个下标；不存在时返回 -1。重复元素不保证返回第一个。
// 最好情况 Θ(1)，最坏情况 Θ(log n)，额外空间 Θ(1)。
template <class T>
int binarySearch(const T a[], int n, const T& x) {
    int left = 0;
    int right = n - 1;
    while (left <= right) {
        // 不使用 (left + right) / 2，避免两个较大下标相加溢出。
        const int middle = left + (right - left) / 2;
        if (x == a[middle]) return middle;
        if (x > a[middle]) left = middle + 1;
        else right = middle - 1;
    }
    return -1;
}

#endif
