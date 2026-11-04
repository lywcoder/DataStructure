#include "accumulate.hpp"
#include "nextPermutation.hpp"
#include "permutation.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <vector>

int main() {
    const int values[] = {2, 3, 4};
    assert(sum(values, 3) == 9);
    assert(product(values, 3) == 24);

    int sequence[] = {1, 2, 3};
    int count = 1;
    while (chapter1::next_permutation(sequence, sequence + 3)) ++count;
    assert(count == 6);
    assert(sequence[0] == 1 && sequence[1] == 2 && sequence[2] == 3);

    int repeated[] = {1, 1, 2};
    std::vector<std::vector<int> > actual;
    do {
        actual.push_back(std::vector<int>(repeated, repeated + 3));
    } while (chapter1::next_permutation(repeated, repeated + 3));
    const std::vector<std::vector<int> > expected{
        {1, 1, 2}, {1, 2, 1}, {2, 1, 1}
    };
    assert(actual == expected);

    int descending[] = {3, 2, 1};
    assert(!chapter1::next_permutation(descending, descending + 3));
    assert(std::is_sorted(descending, descending + 3));

    int suffix[] = {9, 1, 2};
    std::ostringstream printed;
    std::streambuf* previous = std::cout.rdbuf(printed.rdbuf());
    permutation(suffix, 1, 2);
    std::cout.rdbuf(previous);
    assert(printed.str() == "912\n921\n");
}
