#ifndef CHAPTER7_MATRIX_ADT_HPP
#define CHAPTER7_MATRIX_ADT_HPP

#include <cstddef>
#include <stdexcept>

// 所有矩阵共用的行为。公开下标从 1 开始：1 <= i <= rows()。
template <class T>
class matrixADT {
public:
    virtual ~matrixADT() {}
    virtual int rows() const = 0;
    virtual int columns() const = 0;
    virtual T get(int i, int j) const = 0;
    virtual void set(int i, int j, const T& value) = 0;
};

namespace matrix_detail {
inline std::size_t checkedArea(int rows, int columns) {
    if (rows < 0 || columns < 0 || (rows == 0) != (columns == 0))
        throw std::invalid_argument("matrix dimensions must both be zero or positive");
    return static_cast<std::size_t>(rows) * static_cast<std::size_t>(columns);
}
inline void checkIndex(int i, int j, int rows, int columns) {
    if (i < 1 || i > rows || j < 1 || j > columns)
        throw std::out_of_range("matrix index out of range");
}
}

#endif
