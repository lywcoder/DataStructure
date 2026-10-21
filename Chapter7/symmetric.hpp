#ifndef CHAPTER7_SYMMETRIC_HPP
#define CHAPTER7_SYMMETRIC_HPP

#include "matrix.hpp"
#include <stdexcept>
#include <utility>
#include <vector>

// 对称矩阵 a(i,j)==a(j,i)，只保存下三角（含主对角）。
// 第 i 行下三角有 i 项；(i,j), i>=j 对应 i*(i-1)/2 + (j-1)。
template <class T>
class symmetricMatrix : public matrixADT<T> {
public:
    explicit symmetricMatrix(int n = 0)
        : n_(checkSize(n)), lower_(static_cast<std::size_t>(n) *
                                     (static_cast<std::size_t>(n) + 1) / 2, T{}) {}
    int rows() const override { return n_; }
    int columns() const override { return n_; }
    T get(int i, int j) const override {
        matrix_detail::checkIndex(i, j, n_, n_);
        if (i < j) std::swap(i, j);
        return lower_[offset(i, j)];
    }
    void set(int i, int j, const T& value) override {
        matrix_detail::checkIndex(i, j, n_, n_);
        if (i < j) std::swap(i, j);
        lower_[offset(i, j)] = value; // 另一个对称位置自动映射到同一槽位。
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
    static std::size_t offset(int i, int j) {
        return static_cast<std::size_t>(i) * static_cast<std::size_t>(i - 1) / 2 +
               static_cast<std::size_t>(j - 1);
    }
    int n_;
    std::vector<T> lower_;
};

#endif
