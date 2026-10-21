template<class T>
//在有序数学中插入一个元素
void insert(T a[], int& n, const T& x) {
    int i;
    for (i = n - 1; i >= 0 && x < a[i]; i--) {
        a[i + 1] = a[i];
    }
    a[i + 1] = x;
    n++;
}

template<class T>
void insert(T a[], int n, const T& x) {
    //将x插入有序数组a[0:n-1]
    int i;
    for (i = n -1; i >= 0 && x < a[i]; i--) {
        a[i + 1] = a[i];
    }
    a[i + 1] = x;
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