template <class T>
void rank(T a[], int n, int r[]) {
    for (int i = 0; i < n; i++) {
        r[i] = 0;
    }

    //比较所有元素对
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (a[j] <= a[i]) r[i]++;
            else r[j]++;
        }
    }
}

//rank sort or count sort
template <class T>
void rearrange(T a[], int n, int r[]) {
    T *u = new T[n];

    //将a中的元素移到u的正确位置
    for (int i = 0; i < n; i++) {
        u[r[i]] = a[i];
    }

    for (int i = 0; i < n; i++) {
        a[i] = u[i];
    }
    
    delete[] u;
} 
