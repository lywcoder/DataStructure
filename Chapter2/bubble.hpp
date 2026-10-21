template<class T> 
void bubble(T a[], int n) {
    //将a数组中的最大元素移到右边
    for (int i = 0; i < n - 1; i++) {
        if (a[i] > a[i + 1]) swap(a[i], a[i + 1]);
    }
}

template<class T>
void bubbleSort(T a[], int n) {
    for (int i = n; i > 1; i--) {
        bubble(a, i);
    }
}

// //及时终止的冒泡排序
// template<class T>
// bool bubble(T a[], int n) {
//     bool swapped = false;
//     for (int i = 0; i < n - 1; i++) {
//         if (a[i] > a[i + 1]) {
//             swap(a[i], a[i + 1]);
//             swapped = true; //交换
//         }
//     }
//     return swapped;
// }

// template<class T>
// void bubbleSort(T a[], int n) {
//     //及时终止的冒泡排序
//     for (int i = n; i > 1 && bubble(a, i), i--);
// }