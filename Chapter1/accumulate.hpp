#ifndef CHAPTER1_ACCUMULATE_HPP
#define CHAPTER1_ACCUMULATE_HPP

#include <functional>
#include <numeric>

template<class T>
T sum(const T a[], int n) {
    T theSum = 0;
    return std::accumulate(a, a + n, theSum);
}

template<class T>
T product(const T a[], int n) {
    T theProduct = 1;
    return std::accumulate(a, a + n, theProduct, std::multiplies<T>());
}

#endif
