#include "../Chapter5/arrayList.hpp"
#include <ostream>
#include <iostream>

// 异常类定义
class matrixSizeMismatch {
public:
    matrixSizeMismatch(std::string theMessage = "Matrix sizes do not match")
        : message(theMessage) {}
    void outputMessage() { std::cout << message << std::endl; }
private:
    std::string message;
};

template<class T>
struct matrixTerm {
    int row;
    int col;  // 统一使用 col 而非 cols
    T value;
};

template<class T>
class sparseMatrix {
    // 声明友元函数以访问私有成员
    friend std::ostream& operator<<(std::ostream& out, const sparseMatrix<T>& x);
    friend std::istream& operator>>(std::istream& in, sparseMatrix<T>& x);

public:
    sparseMatrix(int theRows = 0, int theCols = 0) : rows(theRows), cols(theCols) {}
    void transpose(sparseMatrix<T>& b);
    void add(sparseMatrix<T>& b, sparseMatrix<T>& c);

private:
    int rows;
    int cols;
    arrayList<matrixTerm<T>> terms;
};

template<class T>
std::ostream& operator<<(std::ostream& out, const sparseMatrix<T>& x) {
    // 输出矩阵的特征
    out << "rows = " << x.rows << " columns = " << x.cols << std::endl;
    out << "nonzero terms = " << x.terms.size() << std::endl;

    // 输出矩阵的项，一行一个
    for (int i = 0; i < x.terms.size(); i++) {
        matrixTerm<T> term = x.terms.get(i);
        out << "a(" << term.row << ',' << term.col << ") = " << term.value << std::endl;
    }
    return out;
}

template<class T>
std::istream& operator>>(std::istream& in, sparseMatrix<T>& x) {
    // 输入矩阵的特征
    int numberOfTerms;
    std::cout << "Enter number of rows, columns, and #terms" << std::endl;
    in >> x.rows >> x.cols >> numberOfTerms;

    // 清空原有项（通过逐个删除）
    while (x.terms.size() > 0) {
        x.terms.erase(0);
    }

    // 输入项
    matrixTerm<T> mTerm;
    for (int i = 0; i < numberOfTerms; i++) {
        std::cout << "Enter row, column, and value of term " << (i + 1) << std::endl;
        in >> mTerm.row >> mTerm.col >> mTerm.value;

        x.terms.insert(i, mTerm);
    }

    return in;
}

template<class T>
void sparseMatrix<T>::transpose(sparseMatrix<T>& b) {
    // 设置转置的矩阵特征
    b.cols = rows;
    b.rows = cols;

    // 清空 b 的原有项
    while (b.terms.size() > 0) {
        b.terms.erase(0);
    }

    // 初始化来实现转置
    int* colSize = new int[cols + 1];
    int* rowNext = new int[cols + 1];

    // 寻找 *this 中每一列的项的数目
    for (int i = 1; i <= cols; i++) {
        colSize[i] = 0;
    }
    for (int i = 0; i < terms.size(); i++) {
        matrixTerm<T> term = terms.get(i);
        colSize[term.col]++;
    }

    // 寻找 b 中每一行的起始点
    rowNext[1] = 0;
    for (int i = 2; i <= cols; i++) {
        rowNext[i] = rowNext[i - 1] + colSize[i - 1];
    }

    // 实施从 *this 到 b 的转置赋值
    matrixTerm<T> mTerm;
    for (int i = 0; i < terms.size(); i++) {
        matrixTerm<T> term = terms.get(i);
        int j = rowNext[term.col]++;
        mTerm.row = term.col;
        mTerm.col = term.row;
        mTerm.value = term.value;
        b.terms.insert(j, mTerm);
    }

    // 释放动态分配的内存
    delete[] colSize;
    delete[] rowNext;
}

template<class T>
void sparseMatrix<T>::add(sparseMatrix<T>& b, sparseMatrix<T>& c) {
    // 检验相容性
    if (rows != b.rows || cols != b.cols) {
        throw matrixSizeMismatch();
    }

    // 设置结果矩阵的特征
    c.rows = rows;
    c.cols = cols;

    // 清空 c 的原有项
    while (c.terms.size() > 0) {
        c.terms.erase(0);
    }

    // 定义 *this 和 b 的索引
    int it = 0;
    int ib = 0;
    int itEnd = terms.size();
    int ibEnd = b.terms.size();

    // 遍历 *this 和 b，把相关的项相加
    while (it < itEnd && ib < ibEnd) {
        matrixTerm<T> termA = terms.get(it);
        matrixTerm<T> termB = b.terms.get(ib);

        int tIndex = termA.row * cols + termA.col;
        int bIndex = termB.row * cols + termB.col;

        if (tIndex < bIndex) {
            // *this 的项在前面，直接加入 c
            c.terms.insert(c.terms.size(), termA);
            it++;
        } else if (tIndex == bIndex) {
            // 两项同在一个位置
            T sum = termA.value + termB.value;
            if (sum != 0) {
                // 和不为 0 时才加入结果
                matrixTerm<T> newTerm;
                newTerm.row = termA.row;
                newTerm.col = termA.col;
                newTerm.value = sum;
                c.terms.insert(c.terms.size(), newTerm);
            }
            it++;
            ib++;
        } else {
            // b 的项在前面，直接加入 c
            c.terms.insert(c.terms.size(), termB);
            ib++;
        }
    }

    // 处理 *this 剩余的项
    while (it < itEnd) {
        matrixTerm<T> term = terms.get(it);
        c.terms.insert(c.terms.size(), term);
        it++;
    }

    // 处理 b 剩余的项
    while (ib < ibEnd) {
        matrixTerm<T> term = b.terms.get(ib);
        c.terms.insert(c.terms.size(), term);
        ib++;
    }
}