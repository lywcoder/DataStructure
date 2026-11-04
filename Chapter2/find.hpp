
#ifndef CHAPTER2_FIND_HPP
#define CHAPTER2_FIND_HPP

template <class T>
int sequentialSearch(T a[], int n, const T& x) {
    for (int i = 0; i < n; ++i)
        if (a[i] == x)
            return i;
    return -1;
}

#endif
