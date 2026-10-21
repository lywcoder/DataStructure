#ifndef CHAPTER7_ROW_SPARSE_HPP
#define CHAPTER7_ROW_SPARSE_HPP

#include "sparse.hpp"
#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <vector>

template <class T>
struct rowEntry {
    int col;
    T value;
    rowEntry() : col(0), value() {}
    rowEntry(int c, const T& v) : col(c), value(v) {}
};

// 多个数组线性表：外层数组按行索引，每一行只保存 (列,非零值)，按列升序。
template <class T>
class rowSparseMatrix : public matrixADT<T> {
public:
    rowSparseMatrix(int rows = 0, int columns = 0)
        : rows_(rows), columns_(columns), rowsData_(checkedRows(rows, columns)), nnz_(0) {}
    int rows() const override { return rows_; }
    int columns() const override { return columns_; }
    std::size_t nonZeros() const { return nnz_; }
    const std::vector<rowEntry<T>>& rowTerms(int i) const {
        if (i < 1 || i > rows_) throw std::out_of_range("row out of range");
        return rowsData_[static_cast<std::size_t>(i - 1)];
    }

    T get(int i, int j) const override {
        matrix_detail::checkIndex(i, j, rows_, columns_);
        const std::vector<rowEntry<T>>& row = rowsData_[static_cast<std::size_t>(i - 1)];
        typename std::vector<rowEntry<T>>::const_iterator it = locate(row, j);
        return it != row.end() && it->col == j ? it->value : T{};
    }
    void set(int i, int j, const T& value) override {
        matrix_detail::checkIndex(i, j, rows_, columns_);
        const T saved(value);
        std::vector<rowEntry<T>>& row = rowsData_[static_cast<std::size_t>(i - 1)];
        typename std::vector<rowEntry<T>>::iterator it = locate(row, j);
        const bool present = it != row.end() && it->col == j;
        if (saved == T{}) {
            if (present) { row.erase(it); --nnz_; }
        } else if (present) {
            it->value = saved;
        } else {
            row.insert(it, rowEntry<T>(j, saved));
            ++nnz_;
        }
    }

    static rowSparseMatrix fromTriple(const sparseMatrix<T>& triples) {
        rowSparseMatrix result(triples.rows(), triples.columns());
        for (const matrixTerm<T>& term : triples.terms()) {
            result.rowsData_[static_cast<std::size_t>(term.row - 1)]
                .push_back(rowEntry<T>(term.col, term.value));
            ++result.nnz_;
        }
        return result;
    }
    sparseMatrix<T> toTriple() const {
        sparseMatrix<T> result(rows_, columns_);
        for (int i = 1; i <= rows_; ++i)
            for (const rowEntry<T>& entry : rowTerms(i))
                result.set(i, entry.col, entry.value);
        return result;
    }
    matrix<T> toDense() const {
        matrix<T> result(rows_, columns_);
        for (int i = 1; i <= rows_; ++i)
            for (const rowEntry<T>& entry : rowTerms(i))
                result(i, entry.col) = entry.value;
        return result;
    }

    rowSparseMatrix operator+(const rowSparseMatrix& other) const {
        checkSameShape(other);
        rowSparseMatrix result(rows_, columns_);
        for (int i = 0; i < rows_; ++i) {
            const std::vector<rowEntry<T>>& a = rowsData_[static_cast<std::size_t>(i)];
            const std::vector<rowEntry<T>>& b = other.rowsData_[static_cast<std::size_t>(i)];
            std::vector<rowEntry<T>>& out = result.rowsData_[static_cast<std::size_t>(i)];
            std::size_t x = 0, y = 0;
            while (x < a.size() && y < b.size()) {
                if (a[x].col < b[y].col) out.push_back(a[x++]);
                else if (b[y].col < a[x].col) out.push_back(b[y++]);
                else {
                    const T sum = a[x].value + b[y].value;
                    if (!(sum == T{})) out.push_back(rowEntry<T>(a[x].col, sum));
                    ++x; ++y;
                }
            }
            while (x < a.size()) out.push_back(a[x++]);
            while (y < b.size()) out.push_back(b[y++]);
            result.nnz_ += out.size();
        }
        return result;
    }

    // 扫描旧行时行号递增；转置后它成为新列号，所以可直接尾插且保持排序。
    rowSparseMatrix transpose() const {
        rowSparseMatrix result(columns_, rows_);
        for (int i = 1; i <= rows_; ++i)
            for (const rowEntry<T>& entry : rowTerms(i)) {
                result.rowsData_[static_cast<std::size_t>(entry.col - 1)]
                    .push_back(rowEntry<T>(i, entry.value));
                ++result.nnz_;
            }
        return result;
    }

    // 稀疏乘法：A 行中每个 a(i,k) 只遍历 B 的第 k 行。
    // 用列号索引的临时数组累计当前结果行，再对触及的列排序输出。
    rowSparseMatrix operator*(const rowSparseMatrix& other) const {
        if (columns_ != other.rows_)
            throw std::invalid_argument("sparse multiplication shape mismatch");
        rowSparseMatrix result(rows_, other.columns_);
        std::vector<T> sums(static_cast<std::size_t>(other.columns_), T{});
        std::vector<char> touchedFlag(static_cast<std::size_t>(other.columns_), 0);
        std::vector<int> touched;
        for (int i = 0; i < rows_; ++i) {
            touched.clear();
            for (const rowEntry<T>& a : rowsData_[static_cast<std::size_t>(i)])
                for (const rowEntry<T>& b : other.rowsData_[static_cast<std::size_t>(a.col - 1)]) {
                    const std::size_t col = static_cast<std::size_t>(b.col - 1);
                    if (!touchedFlag[col]) {
                        touchedFlag[col] = 1;
                        touched.push_back(b.col);
                    }
                    sums[col] = sums[col] + a.value * b.value;
                }
            std::sort(touched.begin(), touched.end());
            std::vector<rowEntry<T>>& out = result.rowsData_[static_cast<std::size_t>(i)];
            for (int col : touched) {
                const std::size_t index = static_cast<std::size_t>(col - 1);
                if (!(sums[index] == T{})) out.push_back(rowEntry<T>(col, sums[index]));
                sums[index] = T{};
                touchedFlag[index] = 0;
            }
            result.nnz_ += out.size();
        }
        return result;
    }

private:
    static std::size_t checkedRows(int rows, int columns) {
        matrix_detail::checkedArea(rows, columns);
        return static_cast<std::size_t>(rows);
    }
    void checkSameShape(const rowSparseMatrix& other) const {
        if (rows_ != other.rows_ || columns_ != other.columns_)
            throw std::invalid_argument("sparse matrix shape mismatch");
    }
    static typename std::vector<rowEntry<T>>::iterator locate(std::vector<rowEntry<T>>& row, int col) {
        return std::lower_bound(row.begin(), row.end(), col,
            [](const rowEntry<T>& entry, int key) { return entry.col < key; });
    }
    static typename std::vector<rowEntry<T>>::const_iterator locate(const std::vector<rowEntry<T>>& row, int col) {
        return std::lower_bound(row.begin(), row.end(), col,
            [](const rowEntry<T>& entry, int key) { return entry.col < key; });
    }
    int rows_;
    int columns_;
    std::vector<std::vector<rowEntry<T>>> rowsData_;
    std::size_t nnz_;
};

template <class T>
std::ostream& operator<<(std::ostream& out, const rowSparseMatrix<T>& value) {
    out << value.rows() << 'x' << value.columns() << " nnz=" << value.nonZeros();
    for (int i = 1; i <= value.rows(); ++i)
        for (const rowEntry<T>& entry : value.rowTerms(i))
            out << "\n(" << i << ',' << entry.col << ")=" << entry.value;
    return out;
}

#endif
