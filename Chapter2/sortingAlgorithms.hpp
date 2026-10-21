#ifndef chapter2_sortingAlgorithms_
#define chapter2_sortingAlgorithms_

#include <utility>
#include <vector>

// 可与原有教材示例同时包含；以下算法统一按 < 升序排列。
namespace chapter2 {

template<class T>
bool isSorted(const T a[], int n) {
    for (int i = 1; i < n; ++i)
        if (a[i] < a[i - 1])
            return false;
    return true;
}

// 一趟冒泡把当前区间的最大值送到最后，并不一定完成整个排序。
template<class T>
void bubble(T a[], int n) {
    for (int i = 0; i < n - 1; ++i)
        if (a[i + 1] < a[i])
            std::swap(a[i], a[i + 1]);
}

template<class T>
void bubbleSort(T a[], int n) {
    for (int size = n; size > 1; --size)
        chapter2::bubble(a, size);
}

template<class T>
bool bubbleWithSwapFlag(T a[], int n) {
    bool swapped = false;
    for (int i = 0; i < n - 1; ++i) {
        if (a[i + 1] < a[i]) {
            std::swap(a[i], a[i + 1]);
            swapped = true;
        }
    }
    return swapped;
}

template<class T>
void bubbleSortEarlyExit(T a[], int n) {
    for (int size = n; size > 1; --size)
        if (!chapter2::bubbleWithSwapFlag(a, size))
            break;
}

template<class T>
int indexOfMax(const T a[], int n) {
    if (n == 0)
        return -1;
    int index = 0;
    for (int i = 1; i < n; ++i)
        if (a[index] < a[i])
            index = i;
    return index;
}

template<class T>
void selectionSort(T a[], int n) {
    for (int size = n; size > 1; --size) {
        int index = chapter2::indexOfMax(a, size);
        if (index != size - 1)
            std::swap(a[index], a[size - 1]);
    }
}

template<class T>
void selectionSortEarlyExit(T a[], int n) {
    for (int size = n; size > 1; --size) {
        int index = 0;
        bool sorted = true;
        for (int i = 1; i < size; ++i) {
            if (a[index] < a[i])
                index = i;
            if (a[i] < a[i - 1])
                sorted = false;
        }
        // 证明整个剩余区间有序，而不是仅判断最大值已经在末尾。
        if (sorted)
            break;
        if (index != size - 1)
            std::swap(a[index], a[size - 1]);
    }
}

// a[0:n) 已有序，a[n] 必须是可写的额外位置；函数不修改 n。
template<class T>
void insertIntoSortedPrefix(T a[], int n, const T& x) {
    // x 可能引用 a 内元素，移动前先保存其值。
    T value = x;
    int position = n;
    while (position > 0 && value < a[position - 1]) {
        a[position] = a[position - 1];
        --position;
    }
    a[position] = value;
}

template<class T>
void insertionSort(T a[], int n) {
    for (int i = 1; i < n; ++i)
        chapter2::insertIntoSortedPrefix(a, i, a[i]);
}

template<class T>
void insertionSortEarlyExit(T a[], int n) {
    for (int i = 1; i < n; ++i) {
        // 当前元素已在正确位置，只跳过本次插入，不能结束外层循环。
        if (!(a[i] < a[i - 1]))
            continue;

        T value = a[i];
        int position = i;
        // 上面的比较已证明至少需要移动一次，避免重复比较。
        do {
            a[position] = a[position - 1];
            --position;
        } while (position > 0 && value < a[position - 1]);
        a[position] = value;
    }
}

// 秩 = 更小的元素个数 + 原序列中位于自己之前的等价元素个数。
// 打破重复值的排名平局，使 r 是 0..n-1 的一个排列并保证稳定性。
template<class T>
void rank(const T a[], int n, int r[]) {
    for (int i = 0; i < n; ++i)
        r[i] = 0;
    for (int i = 1; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (!(a[i] < a[j]))
                ++r[i];
            else
                ++r[j];
        }
    }
}

template<class T>
void rearrange(T a[], int n, const int r[]) {
    if (n < 2)
        return;
    // 复制初始化避免要求 T 必须有默认构造函数。
    std::vector<T> result(a, a + n);
    for (int i = 0; i < n; ++i)
        result[r[i]] = a[i];
    for (int i = 0; i < n; ++i)
        a[i] = result[i];
}

template<class T>
void rankSort(T a[], int n) {
    if (n < 2)
        return;
    std::vector<int> ranks(n);
    chapter2::rank(a, n, ranks.data());
    chapter2::rearrange(a, n, ranks.data());
}

template<class T>
void rankSortEarlyExit(T a[], int n) {
    // 这里只是排序前的整体有序检查，不能在两两排名的中途随意退出。
    if (chapter2::isSorted(a, n))
        return;
    chapter2::rankSort(a, n);
}

} // namespace chapter2

#endif
