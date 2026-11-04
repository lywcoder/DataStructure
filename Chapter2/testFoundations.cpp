#include "find.hpp"
#include "insert.hpp"
#include "ploy.hpp"
#include "rank.hpp"
#include "rearrage.hpp"

#include <cassert>

int main() {
    int values[] = {4, 7, 7, 9};
    assert(sequentialSearch(values, 4, 7) == 1);
    assert(sequentialSearch(values, 4, 4) == 0);
    assert(sequentialSearch(values, 4, 9) == 3);
    assert(sequentialSearch(values, 4, 5) == -1);
    assert(sequentialSearch<int>(nullptr, 0, 5) == -1);
    int coefficients[] = {2, 3, 4};
    assert(polyEval(coefficients, 2, 2) == 24);
    assert(horner(coefficients, 2, 2) == 24);
    assert(polyEval(coefficients, 0, 9) == 2);
    assert(horner(coefficients, 0, 9) == 2);
    assert(polyEval(coefficients, 2, 0) == 2);
    assert(horner(coefficients, 2, 0) == 2);

    int prefix[5] = {1, 3, 5, 0, 0};
    insert(prefix, 3, 4);
    assert(prefix[0] == 1 && prefix[1] == 3 &&
           prefix[2] == 4 && prefix[3] == 5);
    int length = 4;
    insertAndGrow(prefix, length, 2);
    assert(length == 5 && prefix[0] == 1 && prefix[1] == 2 &&
           prefix[2] == 3 && prefix[3] == 4 && prefix[4] == 5);

    int aliased[] = {1, 4, 2};
    insert(aliased, 2, aliased[2]);
    assert(aliased[0] == 1 && aliased[1] == 2 && aliased[2] == 4);

    int ranked[] = {30, 10, 20, 10};
    int ranks[4];
    rank(ranked, 4, ranks);
    assert(ranks[0] == 3 && ranks[1] == 0 &&
           ranks[2] == 2 && ranks[3] == 1);
    rearrangeInPlace(ranked, 4, ranks);
    assert(ranked[0] == 10 && ranked[1] == 10 &&
           ranked[2] == 20 && ranked[3] == 30);
    for (int i = 0; i < 4; ++i) assert(ranks[i] == i);
}
