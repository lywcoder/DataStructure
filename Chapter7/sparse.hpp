#ifndef CHAPTER7_SPARSE_HPP
#define CHAPTER7_SPARSE_HPP

#include "matrix.hpp"
#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

// 一个非零项的坐标和值。公开坐标仍从 1 开始。
template <class T>
struct matrixTerm {
    int row;
    int col;
    T value;
    matrixTerm() : row(0), col(0), value() {}
    matrixTerm(int r, int c, const T& v) : row(r), col(c), value(v) {}
};

// 单个数组线性表：仅保存非零三元组，严格按 (row,col) 升序排列。
// 同一坐标最多出现一次；设置为零时删除该项。
template <class T>
class sparseMatrix : public matrixADT<T> {
public:
    sparseMatrix(int rows = 0, int columns = 0)
        : rows_(rows), columns_(columns) {
        matrix_detail::checkedArea(rows, columns);
    }
    int rows() const override { return rows_; }
    int columns() const override { return columns_; }
    std::size_t nonZeros() const { return terms_.size(); }
    const std::vector<matrixTerm<T>>& terms() const { return terms_; }

    T get(int i, int j) const override {
        matrix_detail::checkIndex(i, j, rows_, columns_);
        typename std::vector<matrixTerm<T>>::const_iterator it = locate(i, j);
        return it != terms_.end() && it->row == i && it->col == j ? it->value : T{};
    }
    void set(int i, int j, const T& value) override {
        matrix_detail::checkIndex(i, j, rows_, columns_);
        const T saved(value); // value 可引用 terms_ 内部元素，先复制以防插入失效。
        typename std::vector<matrixTerm<T>>::iterator it = locate(i, j);
        const bool present = it != terms_.end() && it->row == i && it->col == j;
        if (saved == T{}) {
            if (present) terms_.erase(it);
        } else if (present) {
            it->value = saved;
        } else {
            terms_.insert(it, matrixTerm<T>(i, j, saved));
        }
    }

    static sparseMatrix fromDense(const matrix<T>& dense) {
        sparseMatrix result(dense.rows(), dense.columns());
        for (int i = 1; i <= dense.rows(); ++i)
            for (int j = 1; j <= dense.columns(); ++j)
                if (!(dense(i, j) == T{}))
                    result.terms_.push_back(matrixTerm<T>(i, j, dense(i, j)));
        return result;
    }
    matrix<T> toDense() const {
        matrix<T> result(rows_, columns_);
        for (const matrixTerm<T>& term : terms_) result(term.row, term.col) = term.value;
        return result;
    }

    // 两个已排序三元组序列像归并排序一样线性合并。
    sparseMatrix operator+(const sparseMatrix& other) const {
        if (rows_ != other.rows_ || columns_ != other.columns_)
            throw std::invalid_argument("sparse matrix shape mismatch");
        sparseMatrix result(rows_, columns_);
        result.terms_.reserve(terms_.size() + other.terms_.size());
        std::size_t a = 0, b = 0;
        while (a < terms_.size() && b < other.terms_.size()) {
            if (before(terms_[a], other.terms_[b])) result.terms_.push_back(terms_[a++]);
            else if (before(other.terms_[b], terms_[a])) result.terms_.push_back(other.terms_[b++]);
            else {
                const T sum = terms_[a].value + other.terms_[b].value;
                if (!(sum == T{}))
                    result.terms_.push_back(matrixTerm<T>(terms_[a].row, terms_[a].col, sum));
                ++a;
                ++b;
            }
        }
        while (a < terms_.size()) result.terms_.push_back(terms_[a++]);
        while (b < other.terms_.size()) result.terms_.push_back(other.terms_[b++]);
        return result;
    }

    // 快速转置：先计数每个旧列有多少项，再算新行起点，最后直接定位写入。
    sparseMatrix transpose() const {
        sparseMatrix result(columns_, rows_);
        result.terms_.resize(terms_.size());
        std::vector<std::size_t> next(static_cast<std::size_t>(columns_), 0);
        for (const matrixTerm<T>& term : terms_)
            ++next[static_cast<std::size_t>(term.col - 1)];
        std::size_t start = 0;
        for (std::size_t col = 0; col < next.size(); ++col) {
            const std::size_t count = next[col];
            next[col] = start;
            start += count;
        }
        for (const matrixTerm<T>& term : terms_) {
            const std::size_t position = next[static_cast<std::size_t>(term.col - 1)]++;
            result.terms_[position] = matrixTerm<T>(term.col, term.row, term.value);
        }
        return result;
    }

private:
    static bool before(const matrixTerm<T>& a, const matrixTerm<T>& b) {
        return a.row < b.row || (a.row == b.row && a.col < b.col);
    }
    typename std::vector<matrixTerm<T>>::iterator locate(int i, int j) {
        return std::lower_bound(terms_.begin(), terms_.end(), std::make_pair(i, j),
            [](const matrixTerm<T>& term, const std::pair<int, int>& key) {
                return term.row < key.first || (term.row == key.first && term.col < key.second);
            });
    }
    typename std::vector<matrixTerm<T>>::const_iterator locate(int i, int j) const {
        return std::lower_bound(terms_.begin(), terms_.end(), std::make_pair(i, j),
            [](const matrixTerm<T>& term, const std::pair<int, int>& key) {
                return term.row < key.first || (term.row == key.first && term.col < key.second);
            });
    }
    int rows_;
    int columns_;
    std::vector<matrixTerm<T>> terms_;
};

template <class T>
std::ostream& operator<<(std::ostream& out, const sparseMatrix<T>& value) {
    out << value.rows() << 'x' << value.columns() << " nnz=" << value.nonZeros();
    for (const matrixTerm<T>& term : value.terms())
        out << "\n(" << term.row << ',' << term.col << ")=" << term.value;
    return out;
}

#endif
