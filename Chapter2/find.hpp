
template <class T>
int sequentialSearch(T a[], int n, const T& x) {
    int i;
    for (i = 0; i < n && a[i] != x; i++) {
        if (i == n) return -1;
        else return i;
    }
}
