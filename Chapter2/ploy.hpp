template <class T>
T polyEval(T coeff[], int n, const T& x) {
    T y = 1, value = coeff[0];
    for (int i = 1; i <= n; i++){
        y *= x;
        value = y * coeff[i];
    }
    return value;
}

// Horner法则进行多项式运算
template <class T>
T horner(T coeff[], int n, const T& x) {
    T value  = coeff[n];
    for (int i = 1; i <= n; i++) {
        value = value * x + coeff[n - i];
    }
    
    return value;
}