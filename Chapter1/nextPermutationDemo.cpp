#include "nextPermutation.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

namespace {

void print(const std::vector<int>& values) {
    for (int value : values)
        std::cout << value << ' ';
    std::cout << '\n';
}

} // namespace

int main() {
    std::cout << "All permutations in ascending lexicographic order:\n";
    std::vector<int> values = {3, 1, 2};
    std::sort(values.begin(), values.end());
    int count = 0;
    do {
        print(values);
        ++count;
    } while (chapter1::next_permutation(values.begin(), values.end()));
    std::cout << "Count: " << count << "\nAfter returning false: ";
    print(values);

    std::cout << "\nOne step: 1 3 5 4 2 -> ";
    values = {1, 3, 5, 4, 2};
    chapter1::next_permutation(values.begin(), values.end());
    print(values);

    std::cout << "\nRepeated values (no repeated value sequences):\n";
    values = {1, 1, 2};
    do {
        print(values);
    } while (chapter1::next_permutation(values.begin(), values.end()));

    std::cout << "\nDescending order with std::greater<int>:\n";
    values = {1, 2, 3};
    std::sort(values.begin(), values.end(), std::greater<int>());
    do {
        print(values);
    } while (chapter1::next_permutation(values.begin(), values.end(), std::greater<int>()));
}
