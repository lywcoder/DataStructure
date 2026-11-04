#ifndef CHAPTER2_PLOY_HPP
#define CHAPTER2_PLOY_HPP

#include <stdexcept>

// n is the degree: coeff has n + 1 entries, in ascending power order.
template <class T>
T polyEval(T coeff[], int n, const T& x) {
    if (n < 0) throw std::invalid_argument("Degree must be nonnegative");
    T y = 1, value = coeff[0];
    for (int i = 1; i <= n; i++){
        y *= x;
        value += y * coeff[i];
    }
    return value;
}

// Horner法则进行多项式运算
template <class T>
T horner(T coeff[], int n, const T& x) {
    if (n < 0) throw std::invalid_argument("Degree must be nonnegative");
    T value  = coeff[n];
    for (int i = 1; i <= n; i++) {
        value = value * x + coeff[n - i];
    }
    
    return value;
}

#endif
