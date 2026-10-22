#include "diagonal.hpp"
#include "rowSparse.hpp"
#include "symmetric.hpp"
#include "tridiagonal.hpp"

#include <iostream>

int main() {
    matrix<int> dense(2, 3);
    dense(1, 1) = 4;
    dense(1, 3) = 7;
    dense(2, 2) = -2;
    std::cout << "Dense 2x3:\n" << dense << "\n\n";

    diagonalMatrix<int> diagonal(3);
    diagonal.set(2, 2, 9);
    std::cout << "Diagonal:\n" << diagonal.toDense() << "\n\n";

    tridiagonalMatrix<int> tri(3);
    tri.set(1, 2, 5);
    tri.set(2, 2, 8);
    tri.set(3, 2, 6);
    std::cout << "Tridiagonal:\n" << tri.toDense() << "\n\n";

    symmetricMatrix<int> symmetric(3);
    symmetric.set(1, 3, 4);
    std::cout << "Symmetric (1,3) and (3,1) share one slot:\n"
              << symmetric.toDense() << "\n\n";

    const sparseMatrix<int> triples = sparseMatrix<int>::fromDense(dense);
    const rowSparseMatrix<int> byRows = rowSparseMatrix<int>::fromTriple(triples);
    std::cout << "One list of triples:\n" << triples << "\n\n";
    std::cout << "Several row lists:\n" << byRows << "\n\n";
    std::cout << "Fast transpose:\n" << triples.transpose() << "\n\n";

    rowSparseMatrix<int> right(3, 2);
    right.set(1, 1, 2);
    right.set(2, 2, 3);
    right.set(3, 1, -1);
    std::cout << "Sparse multiplication result:\n" << (byRows * right).toDense() << '\n';
}
