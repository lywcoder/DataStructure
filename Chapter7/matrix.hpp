#ifndef CHAPTER7_MATRIX_HPP
#define CHAPTER7_MATRIX_HPP

#include "matrixADT.hpp"
#include <ostream>
#include <stdexcept>
#include <vector>

// 稠密矩阵：将 r×c 个元素按行优先放入一个连续数组。
// 公开 (i,j) 对应内部下标 (i-1)*columns + (j-1)。
template <class T>
class matrix : public matrixADT<T> {
public:
    matrix(int rows = 0, int columns = 0)
        : rows_(rows), columns_(columns), data_(matrix_detail::checkedArea(rows, columns), T{}) {}

    int rows() const override { return rows_; }
    int columns() const override { return columns_; }

    T get(int i, int j) const override { return (*this)(i, j); }
    void set(int i, int j, const T& value) override { (*this)(i, j) = value; }

    T& operator()(int i, int j) {
        matrix_detail::checkIndex(i, j, rows_, columns_);
        return data_[offset(i, j)];
    }
    const T& operator()(int i, int j) const {
        matrix_detail::checkIndex(i, j, rows_, columns_);
        return data_[offset(i, j)];
    }

    matrix transpose() const {
        matrix result(columns_, rows_);
        for (int i = 1; i <= rows_; ++i)
            for (int j = 1; j <= columns_; ++j)
                result(j, i) = (*this)(i, j);
        return result;
    }

    matrix operator+() const { return *this; }
    matrix operator-() const {
        matrix result(rows_, columns_);
        for (std::size_t i = 0; i < data_.size(); ++i) result.data_[i] = -data_[i];
        return result;
    }
    matrix operator+(const matrix& other) const {
        checkSameShape(other);
        matrix result(rows_, columns_);
        for (std::size_t i = 0; i < data_.size(); ++i)
            result.data_[i] = data_[i] + other.data_[i];
        return result;
    }
    matrix operator-(const matrix& other) const {
        checkSameShape(other);
        matrix result(rows_, columns_);
        for (std::size_t i = 0; i < data_.size(); ++i)
            result.data_[i] = data_[i] - other.data_[i];
        return result;
    }
    matrix& operator+=(const matrix& other) {
        *this = *this + other; // 先形成完整结果，支持 self += self。
        return *this;
    }
    matrix& operator+=(const T& scalar) {
        for (T& value : data_) value = value + scalar;
        return *this;
    }

    matrix operator*(const matrix& other) const {
        if (columns_ != other.rows_)
            throw std::invalid_argument("matrix multiplication shape mismatch");
        matrix result(rows_, other.columns_);
        for (int i = 1; i <= rows_; ++i)
            for (int k = 1; k <= columns_; ++k)
                for (int j = 1; j <= other.columns_; ++j)
                    result(i, j) = result(i, j) + (*this)(i, k) * other(k, j);
        return result;
    }

private:
    std::size_t offset(int i, int j) const {
        return static_cast<std::size_t>(i - 1) * static_cast<std::size_t>(columns_) +
               static_cast<std::size_t>(j - 1);
    }
    void checkSameShape(const matrix& other) const {
        if (rows_ != other.rows_ || columns_ != other.columns_)
            throw std::invalid_argument("matrix shape mismatch");
    }
    int rows_;
    int columns_;
    std::vector<T> data_;
};

template <class T>
std::ostream& operator<<(std::ostream& out, const matrix<T>& value) {
    for (int i = 1; i <= value.rows(); ++i) {
        for (int j = 1; j <= value.columns(); ++j) {
            if (j != 1) out << ' ';
            out << value(i, j);
        }
        if (i != value.rows()) out << '\n';
    }
    return out;
}

#endif
