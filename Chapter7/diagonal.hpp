#ifndef CHAPTER7_DIAGONAL_HPP
#define CHAPTER7_DIAGONAL_HPP

#include "matrix.hpp"
#include <stdexcept>
#include <vector>

// n×n 对角矩阵只保存 a(1,1), a(2,2), ... a(n,n)。
template <class T>
class diagonalMatrix : public matrixADT<T> {
public:
    explicit diagonalMatrix(int n = 0) : n_(checkSize(n)), diagonal_(static_cast<std::size_t>(n), T{}) {}
    int rows() const override { return n_; }
    int columns() const override { return n_; }
    T get(int i, int j) const override {
        matrix_detail::checkIndex(i, j, n_, n_);
        return i == j ? diagonal_[static_cast<std::size_t>(i - 1)] : T{};
    }
    void set(int i, int j, const T& value) override {
        matrix_detail::checkIndex(i, j, n_, n_);
        if (i == j) diagonal_[static_cast<std::size_t>(i - 1)] = value;
        else if (!(value == T{}))
            throw std::invalid_argument("off-diagonal element must be zero");
    }
    matrix<T> toDense() const {
        matrix<T> result(n_, n_);
        for (int i = 1; i <= n_; ++i) result(i, i) = get(i, i);
        return result;
    }
private:
    static int checkSize(int n) {
        if (n < 0) throw std::invalid_argument("negative matrix size");
        return n;
    }
    int n_;
    std::vector<T> diagonal_;
};

#endif
