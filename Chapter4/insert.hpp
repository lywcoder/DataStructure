#ifndef CHAPTER4_INSERT_HPP
#define CHAPTER4_INSERT_HPP

// 将 x 插入容量至少为 n+1 的有序数组前缀，并把长度增加 1。
template <class T>
void insert(T a[], int& n, const T& x) {
    const T value = x; // x 可以引用数组内的元素，先保存它。
    int i = n - 1;
    while (i >= 0 && value < a[i]) {
        a[i + 1] = a[i];
        --i;
    }
    a[i + 1] = value;
    ++n;
}

// 直接移动有序前缀。不能把外层循环变量 i 传给会修改长度的 insert。
template <class T>
void insertionSort(T a[], int n) {
    for (int i = 1; i < n; ++i) {
        const T value = a[i];
        int j = i - 1;
        while (j >= 0 && value < a[j]) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = value;
    }
}

#endif
