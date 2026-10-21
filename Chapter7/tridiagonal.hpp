#ifndef CHAPTER7_TRIDIAGONAL_HPP
#define CHAPTER7_TRIDIAGONAL_HPP

#include "matrix.hpp"
#include <stdexcept>
#include <vector>

// 三对角矩阵仅保存 |i-j|<=1 的位置；存储顺序：下对角、主对角、上对角。
template <class T>
class tridiagonalMatrix : public matrixADT<T> {
public:
    explicit tridiagonalMatrix(int n = 0)
        : n_(checkSize(n)), values_(n == 0 ? 0 : 3 * static_cast<std::size_t>(n) - 2, T{}) {}
    int rows() const override { return n_; }
    int columns() const override { return n_; }
    T get(int i, int j) const override {
        matrix_detail::checkIndex(i, j, n_, n_);
        return inBand(i, j) ? values_[offset(i, j)] : T{};
    }
    void set(int i, int j, const T& value) override {
        matrix_detail::checkIndex(i, j, n_, n_);
        if (inBand(i, j)) values_[offset(i, j)] = value;
        else if (!(value == T{}))
            throw std::invalid_argument("element outside tridiagonal band must be zero");
    }
    matrix<T> toDense() const {
        matrix<T> result(n_, n_);
        for (int i = 1; i <= n_; ++i)
            for (int j = 1; j <= n_; ++j)
                result(i, j) = get(i, j);
        return result;
    }
private:
    static int checkSize(int n) {
        if (n < 0) throw std::invalid_argument("negative matrix size");
        return n;
    }
    static bool inBand(int i, int j) { return i == j || i == j + 1 || j == i + 1; }
    std::size_t offset(int i, int j) const {
        if (i == j + 1) return static_cast<std::size_t>(i - 2); // 下对角
        if (i == j) return static_cast<std::size_t>(n_ - 1 + i - 1); // 主对角
        return 2 * static_cast<std::size_t>(n_) - 1 + static_cast<std::size_t>(i - 1); // 上对角
    }
    int n_;
    std::vector<T> values_;
};

#endif
