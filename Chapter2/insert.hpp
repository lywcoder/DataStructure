#ifndef CHAPTER2_INSERT_HPP
#define CHAPTER2_INSERT_HPP

template<class T>
void insert(T a[], int n, const T& x) {
    //将x插入有序数组a[0:n-1]
    const T value = x; // x 可能引用 a[n]，移动前必须保存。
    int i;
    for (i = n -1; i >= 0 && value < a[i]; i--) {
        a[i + 1] = a[i];
    }
    a[i + 1] = value;
}

// 向有序前缀插入并同步增加调用者保存的长度。
// 另取名字以避免 int 与 int& 重载在左值实参上产生二义性。
template<class T>
void insertAndGrow(T a[], int& n, const T& x) {
    const T value = x;
    insert(a, n, value);
    ++n;
}

template<class T>
void insertionSort(T a[], int n) {
    for (int i = 1; i < n; i++) {
        T t = a[i];
        insert(a, i, t);
    }
}

//另一种插入排序
// template<class T>
// void insertionSort(T a[], int n) {
//     for (int i = 1; i < n; i++) {
//         T t = a[i];
//         int j;
//         for (j = i - 1; j >= 0 && t < a[j]; j--)
//             a[j + 1] = a[j];
//         a[j + 1] = t;
//     }
// }

#endif
