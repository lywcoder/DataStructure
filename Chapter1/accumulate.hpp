#include <numeric>
#include <functional>

template<class T>
T sum(T a[], int n) {
    T theSum = 0;
    return std::accumulate(a, a + n, theSum);
}

template<class T>
T product(T a[], int n) {
    T theProduct = 1;
    return accumulate(a, a + n, theProduct, std::multiplies<T>());
}
