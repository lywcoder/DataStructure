#include "diagonal.hpp"
#include "rowSparse.hpp"
#include "symmetric.hpp"
#include "tridiagonal.hpp"

#include <cassert>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

template <class A, class B>
void sameValues(const A& a, const B& b) {
    assert(a.rows() == b.rows() && a.columns() == b.columns());
    for (int i = 1; i <= a.rows(); ++i)
        for (int j = 1; j <= a.columns(); ++j)
            assert(a.get(i, j) == b.get(i, j));
}

void checkDense() {
    matrix<int> a(2, 3);
    int value = 1;
    for (int i = 1; i <= 2; ++i)
        for (int j = 1; j <= 3; ++j)
            a(i, j) = value++;
    assert(a.rows() == 2 && a.columns() == 3);
    assert(a.get(2, 3) == 6 && a.transpose()(3, 2) == 6);
    const matrix<int> copied = a;
    a.set(1, 1, 9);
    assert(copied.get(1, 1) == 1);
    a = copied;
    assert((a + a).get(2, 3) == 12);
    assert((a - a).get(1, 2) == 0);
    assert((-a).get(2, 3) == -6);
    a += a;
    assert(a.get(1, 1) == 2);
    a += 3;
    assert(a.get(1, 1) == 5);

    matrix<int> b(3, 2);
    b(1, 1) = 1; b(1, 2) = 2;
    b(2, 1) = 3; b(2, 2) = 4;
    b(3, 1) = 5; b(3, 2) = 6;
    const matrix<int> product = copied * b;
    assert(product.rows() == 2 && product.columns() == 2);
    assert(product(1, 1) == 22 && product(1, 2) == 28);
    assert(product(2, 1) == 49 && product(2, 2) == 64);

    matrix<int> empty;
    assert(empty.rows() == 0 && empty.columns() == 0);
    const sparseMatrix<int> emptySparse = sparseMatrix<int>::fromDense(empty);
    assert(emptySparse.transpose().nonZeros() == 0);
    assert(rowSparseMatrix<int>::fromTriple(emptySparse).transpose().nonZeros() == 0);
    bool threw = false;
    try { matrix<int> invalid(0, 3); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { a.get(0, 1); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);
    threw = false;
    try { a + b; } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { b * b; } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
}

void checkSpecial() {
    diagonalMatrix<int> diagonal(3);
    matrixADT<int>& asADT = diagonal;
    asADT.set(2, 2, 7);
    assert(asADT.get(2, 2) == 7 && asADT.get(1, 2) == 0);
    sameValues(diagonal, diagonal.toDense());
    bool threw = false;
    try { diagonal.set(1, 2, 9); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    tridiagonalMatrix<int> tri(4);
    for (int i = 1; i <= 4; ++i) {
        tri.set(i, i, 10 + i);
        if (i < 4) {
            tri.set(i + 1, i, 20 + i);
            tri.set(i, i + 1, 30 + i);
        }
    }
    assert(tri.get(4, 3) == 23 && tri.get(3, 4) == 33);
    assert(tri.get(1, 4) == 0);
    sameValues(tri, tri.toDense());
    threw = false;
    try { tri.set(1, 4, 1); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    symmetricMatrix<int> sym(3);
    sym.set(1, 3, 8);
    sym.set(2, 1, 5);
    assert(sym.get(3, 1) == 8 && sym.get(1, 2) == 5);
    sym.set(3, 1, 4);
    assert(sym.get(1, 3) == 4);
    sameValues(sym, sym.toDense());
}

void checkSparseExamples() {
    sparseMatrix<int> a(3, 4);
    a.set(3, 2, 5);
    a.set(1, 4, 7);
    a.set(2, 1, -3);
    a.set(3, 2, 6); // 更新而非重复加入。
    assert(a.nonZeros() == 3 && a.get(1, 1) == 0);
    assert(a.terms()[0].row == 1 && a.terms()[1].row == 2);
    a.set(2, 1, 0); // 写零会删除三元组。
    assert(a.nonZeros() == 2 && a.get(2, 1) == 0);
    const sparseMatrix<int> at = a.transpose();
    assert(at.rows() == 4 && at.columns() == 3 && at.get(2, 3) == 6);
    assert(at.get(4, 1) == 7);
    sameValues(a, at.transpose());

    sparseMatrix<int> neg(3, 4);
    neg.set(1, 4, -7);
    const sparseMatrix<int> sum = a + neg;
    assert(sum.nonZeros() == 1 && sum.get(3, 2) == 6);

    rowSparseMatrix<int> rows = rowSparseMatrix<int>::fromTriple(a);
    assert(rows.nonZeros() == a.nonZeros());
    sameValues(rows, a);
    sameValues(rows.toTriple(), a);
    sameValues(rows.transpose(), at);
    assert(rows.rowTerms(2).empty());
    rows.set(1, 4, 0);
    assert(rows.nonZeros() == 1 && rows.get(1, 4) == 0);
    rows.set(1, 4, 7);
    assert(rows.nonZeros() == 2 && rows.get(1, 4) == 7);

    rowSparseMatrix<int> left(1, 2), right(2, 1);
    left.set(1, 1, 3);
    left.set(1, 2, -3);
    right.set(1, 1, 2);
    right.set(2, 1, 2);
    assert((left * right).nonZeros() == 0); // 非零乘积抵消为零。

    std::ostringstream out;
    out << a;
    assert(out.str().find("(1,4)=7") != std::string::npos);
}

void checkRandomSparse() {
    std::mt19937 rng(2026);
    for (int trial = 0; trial < 150; ++trial) {
        const int r = 1 + static_cast<int>(rng() % 6);
        const int inner = 1 + static_cast<int>(rng() % 6);
        const int c = 1 + static_cast<int>(rng() % 6);
        matrix<int> denseA(r, inner), denseB(r, inner), denseC(inner, c);
        for (int i = 1; i <= r; ++i)
            for (int j = 1; j <= inner; ++j) {
                denseA(i, j) = static_cast<int>(rng() % 5) - 2;
                denseB(i, j) = static_cast<int>(rng() % 5) - 2;
            }
        for (int i = 1; i <= inner; ++i)
            for (int j = 1; j <= c; ++j)
                denseC(i, j) = static_cast<int>(rng() % 5) - 2;

        const sparseMatrix<int> tripleA = sparseMatrix<int>::fromDense(denseA);
        const sparseMatrix<int> tripleB = sparseMatrix<int>::fromDense(denseB);
        const rowSparseMatrix<int> rowA = rowSparseMatrix<int>::fromTriple(tripleA);
        const rowSparseMatrix<int> rowB = rowSparseMatrix<int>::fromTriple(tripleB);
        const rowSparseMatrix<int> rowC = rowSparseMatrix<int>::fromTriple(
            sparseMatrix<int>::fromDense(denseC));
        sameValues(tripleA.toDense(), denseA);
        sameValues(rowA.toDense(), denseA);
        sameValues(tripleA.transpose(), denseA.transpose());
        sameValues(rowA.transpose(), denseA.transpose());
        sameValues(tripleA + tripleB, denseA + denseB);
        sameValues(rowA + rowB, denseA + denseB);
        sameValues(rowA * rowC, denseA * denseC);
    }
}

int main() {
    checkDense();
    checkSpecial();
    checkSparseExamples();
    checkRandomSparse();
}
