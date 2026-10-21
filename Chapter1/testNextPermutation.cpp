#include "nextPermutation.hpp"
#include "nextPermutation.hpp" // 验证头文件可重复包含。
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace {

template<class Compare>
void checkOneStep(const std::vector<int>& input, Compare comp) {
    std::vector<int> actual(input);
    std::vector<int> expected(input);
    bool result = chapter1::next_permutation(actual.begin(), actual.end(), comp);
    bool reference = std::next_permutation(expected.begin(), expected.end(), comp);
    assert(result == reference);
    assert(actual == expected);
}

void testExhaustiveOneStep() {
    // 穷举长度 0..7、值来自 {0, 1, 2} 的 3280 个序列，包括大量重复值。
    int sequenceCount = 1;
    int checked = 0;
    for (int length = 0; length <= 7; ++length) {
        for (int code = 0; code < sequenceCount; ++code) {
            std::vector<int> input(length);
            int remaining = code;
            for (int i = 0; i < length; ++i) {
                input[i] = remaining % 3;
                remaining /= 3;
            }
            checkOneStep(input, std::less<int>());
            checkOneStep(input, std::greater<int>());

            // 默认重载也单独对照标准库默认重载。
            std::vector<int> actual(input);
            std::vector<int> expected(input);
            bool result = chapter1::next_permutation(actual.begin(), actual.end());
            bool reference = std::next_permutation(expected.begin(), expected.end());
            assert(result == reference);
            assert(actual == expected);
            ++checked;
        }
        sequenceCount *= 3;
    }
    assert(checked == 3280);
    checkOneStep({4, -2, 0, 4, -2}, std::less<int>());
}

void testEnumeration() {
    for (const std::vector<int>& initial :
         {std::vector<int>{1, 2, 3}, std::vector<int>{1, 1, 2},
          std::vector<int>{1, 1, 2, 2, 3}}) {
        std::vector<int> actual(initial);
        std::vector<int> expected(initial);
        int count = 0;
        while (true) {
            assert(actual == expected);
            ++count;
            bool result = chapter1::next_permutation(actual.begin(), actual.end());
            bool reference = std::next_permutation(expected.begin(), expected.end());
            assert(result == reference);
            if (!result)
                break;
        }
        assert(actual == initial); // 最大排列之后恢复最小排列。
        assert(actual == expected);
        if (initial.size() == 5)
            assert(count == 30);
        else
            assert(count == (initial[0] == initial[1] ? 3 : 6));
    }
}

void testBidirectionalIteratorsAndSubrange() {
    std::list<int> actual = {1, 3, 5, 4, 2};
    std::list<int> expected(actual);
    bool result = chapter1::next_permutation(actual.begin(), actual.end());
    bool reference = std::next_permutation(expected.begin(), expected.end());
    assert(result == reference);
    assert(actual == expected);
    assert((actual == std::list<int>{1, 4, 2, 3, 5}));

    int values[] = {99, 1, 3, 2, 88};
    result = chapter1::next_permutation(values + 1, values + 4);
    assert(result);
    assert(values[0] == 99 && values[4] == 88);
    assert(values[1] == 2 && values[2] == 1 && values[3] == 3);

    std::string letters = "aba";
    std::string expectedLetters(letters);
    result = chapter1::next_permutation(letters.begin(), letters.end());
    reference = std::next_permutation(expectedLetters.begin(), expectedLetters.end());
    assert(result == reference);
    assert(letters == expectedLetters);
}

struct Item {
    explicit Item(int rank) : rank(rank) {}
    int rank;
};

void testCustomComparison() {
    // Item 没有默认构造函数，也没有 operator<。
    std::vector<Item> actual = {Item(1), Item(3), Item(2)};
    std::vector<Item> expected(actual);
    auto comp = [](const Item& left, const Item& right) {
        return left.rank < right.rank;
    };
    bool result = chapter1::next_permutation(actual.begin(), actual.end(), comp);
    bool reference = std::next_permutation(expected.begin(), expected.end(), comp);
    assert(result == reference);
    for (std::size_t i = 0; i < actual.size(); ++i)
        assert(actual[i].rank == expected[i].rank);
}

void testMoveOnlyElements() {
    std::vector<std::unique_ptr<int>> actual;
    std::vector<std::unique_ptr<int>> expected;
    for (int value : {1, 2, 3}) {
        actual.emplace_back(new int(value));
        expected.emplace_back(new int(value));
    }
    auto comp = [](const std::unique_ptr<int>& left,
                   const std::unique_ptr<int>& right) { return *left < *right; };
    int count = 0;
    while (true) {
        for (std::size_t i = 0; i < actual.size(); ++i)
            assert(*actual[i] == *expected[i]);
        ++count;
        bool result = chapter1::next_permutation(actual.begin(), actual.end(), comp);
        bool reference = std::next_permutation(expected.begin(), expected.end(), comp);
        assert(result == reference);
        if (!result)
            break;
    }
    assert(count == 6);
    for (std::size_t i = 0; i < actual.size(); ++i)
        assert(*actual[i] == static_cast<int>(i + 1));
}

} // namespace

int main() {
    testExhaustiveOneStep();
    testEnumeration();
    testBidirectionalIteratorsAndSubrange();
    testCustomComparison();
    testMoveOnlyElements();
    std::cout << "All next_permutation tests passed (3280 input sequences).\n";
}
