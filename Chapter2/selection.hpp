template<class T>
int indexOfMax(T a[], int n) {
    int indexOfMax = 0;
    for (int i = 0; i < n; i++) {
        if (a[indexOfMax] < a[i]) {
            indexOfMax = i;
        }
    }

    return indexOfMax;
}

template <class T>
void selectionSort(T a[], int n) {
    for (int size = n; size > 1; size--) {
        int j = indexOfMax(a, size);
        swap(a[j], a[size - 1]);
    }
}

//及时终止的选择排序
// template<class T>
// void selectionSort(T a[], int n) {
//     bool sorted = false;
//     for (int size = n; !sorted && (size > 1); size--) {
//         int indexOfMax = 0;
//         sorted = true;
//         //查找最大元素
//         for (int i  = 1; i < size; i++) {
//             if (a[indexOfMax] <= a[i]) indexOfMax = i;
//             else sorted = false;
//         }

//         swap(a[indexOfMax], a[size - 1]);
//     }
// }