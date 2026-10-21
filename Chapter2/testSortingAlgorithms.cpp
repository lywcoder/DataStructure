#include "sortingAlgorithms.hpp"
#include "sortingAlgorithms.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

namespace {

struct Algorithm {
    const char* name;
    void (*sort)(int[], int);
};

const Algorithm algorithms[] = {
    {"bubble", chapter2::bubbleSort<int>},
    {"bubble early", chapter2::bubbleSortEarlyExit<int>},
    {"selection", chapter2::selectionSort<int>},
    {"selection early", chapter2::selectionSortEarlyExit<int>},
    {"insertion", chapter2::insertionSort<int>},
    {"insertion early", chapter2::insertionSortEarlyExit<int>},
    {"rank", chapter2::rankSort<int>},
    {"rank early", chapter2::rankSortEarlyExit<int>}
};

void checkAll(const std::vector<int>& input) {
    std::vector<int> expected(input);
    std::sort(expected.begin(), expected.end());
    for (const Algorithm& algorithm : algorithms) {
        std::vector<int> actual(input);
        algorithm.sort(actual.data(), static_cast<int>(actual.size()));
        assert(actual == expected);
    }
}

void testExhaustiveAndRandom() {
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
            checkAll(input);
            ++checked;
        }
        sequenceCount *= 3;
    }
    assert(checked == 3280);

    std::mt19937 generator(42);
    std::uniform_int_distribution<int> lengths(0, 60);
    std::uniform_int_distribution<int> values(-20, 20);
    for (int trial = 0; trial < 200; ++trial) {
        std::vector<int> input(lengths(generator));
        for (int& value : input)
            value = values(generator);
        checkAll(input);
    }
    checkAll({3, 1, 2, 4});
    checkAll({1, 2, 4, 3});
    checkAll({5, 4, 3, 2, 1});
    checkAll({1, 2, 3, 4, 5});
    checkAll({2, 2, 2, 2});

    for (const Algorithm& algorithm : algorithms)
        algorithm.sort(nullptr, 0);
}

struct Record {
    int key;
    int identity;
    Record(int key, int identity) : key(key), identity(identity) {}
    bool operator<(const Record& other) const { return key < other.key; }
};

void testStability() {
    const std::vector<Record> input = {
        Record(2, 0), Record(1, 1), Record(2, 2), Record(1, 3), Record(2, 4)
    };
    std::vector<Record> expected(input);
    std::stable_sort(expected.begin(), expected.end());
    typedef void (*Sort)(Record[], int);
    const Sort stableSorts[] = {
        chapter2::bubbleSort<Record>, chapter2::bubbleSortEarlyExit<Record>,
        chapter2::insertionSort<Record>, chapter2::insertionSortEarlyExit<Record>,
        chapter2::rankSort<Record>, chapter2::rankSortEarlyExit<Record>
    };
    for (Sort sort : stableSorts) {
        std::vector<Record> actual(input);
        sort(actual.data(), static_cast<int>(actual.size()));
        for (std::size_t i = 0; i < actual.size(); ++i) {
            assert(actual[i].key == expected[i].key);
            assert(actual[i].identity == expected[i].identity);
        }
    }

    // 选择排序不保证稳定：原来的两个 2 的次序被非相邻交换改变。
    const Sort selectionSorts[] = {
        chapter2::selectionSort<Record>, chapter2::selectionSortEarlyExit<Record>
    };
    for (Sort sort : selectionSorts) {
        std::vector<Record> actual = {Record(2, 0), Record(2, 1), Record(1, 2)};
        sort(actual.data(), static_cast<int>(actual.size()));
        assert(actual[0].key == 1);
        assert(actual[1].identity == 1 && actual[2].identity == 0);
    }
}

void testHelpers() {
    int values[] = {3, 1, 4, 1, 5, 2};
    int ranks[6];
    chapter2::rank(values, 6, ranks);
    const int expectedRanks[] = {3, 0, 4, 1, 5, 2};
    for (int i = 0; i < 6; ++i)
        assert(ranks[i] == expectedRanks[i]);
    chapter2::rearrange(values, 6, ranks);
    assert(chapter2::isSorted(values, 6));
    for (int i = 0; i < 6; ++i)
        assert(ranks[i] == expectedRanks[i]);
    assert(chapter2::indexOfMax(values, 6) == 5);
    assert(chapter2::indexOfMax<int>(nullptr, 0) == -1);

    int prefix[] = {1, 3, 5, 0};
    chapter2::insertIntoSortedPrefix(prefix, 3, 4);
    assert(prefix[0] == 1 && prefix[1] == 3 && prefix[2] == 4 && prefix[3] == 5);
    int alias[] = {1, 3, 5, 0};
    chapter2::insertIntoSortedPrefix(alias, 3, alias[1]);
    assert(alias[0] == 1 && alias[1] == 3 && alias[2] == 3 && alias[3] == 5);

    int onePass[] = {3, 2, 1};
    chapter2::bubble(onePass, 3);
    assert(onePass[2] == 3);
    assert(!chapter2::isSorted(onePass, 3));
    assert(chapter2::bubbleWithSwapFlag(onePass, 3));
    assert(!chapter2::bubbleWithSwapFlag(onePass, 3));
}

struct Counted {
    static int comparisons;
    static int copies;
    int value;
    explicit Counted(int value) : value(value) {}
    Counted(const Counted& other) : value(other.value) { ++copies; }
    Counted& operator=(const Counted& other) {
        value = other.value;
        ++copies;
        return *this;
    }
    bool operator<(const Counted& other) const {
        ++comparisons;
        return value < other.value;
    }
};

int Counted::comparisons = 0;
int Counted::copies = 0;

struct Counts {
    int comparisons;
    int copies;
};

Counts countOperations(void (*sort)(Counted[], int), const std::vector<Counted>& input) {
    std::vector<Counted> actual(input);
    Counted::comparisons = Counted::copies = 0;
    sort(actual.data(), static_cast<int>(actual.size()));
    return Counts{Counted::comparisons, Counted::copies};
}

void testEarlyExitCosts() {
    const int n = 32;
    const int quadratic = n * (n - 1) / 2;
    std::vector<Counted> sorted;
    for (int i = 0; i < n; ++i)
        sorted.emplace_back(i);

    Counts bubble = countOperations(chapter2::bubbleSort<Counted>, sorted);
    Counts bubbleEarly = countOperations(chapter2::bubbleSortEarlyExit<Counted>, sorted);
    Counts selection = countOperations(chapter2::selectionSort<Counted>, sorted);
    Counts selectionEarly = countOperations(chapter2::selectionSortEarlyExit<Counted>, sorted);
    Counts insertion = countOperations(chapter2::insertionSort<Counted>, sorted);
    Counts insertionEarly = countOperations(chapter2::insertionSortEarlyExit<Counted>, sorted);
    Counts rank = countOperations(chapter2::rankSort<Counted>, sorted);
    Counts rankEarly = countOperations(chapter2::rankSortEarlyExit<Counted>, sorted);

    assert(bubble.comparisons == quadratic && bubbleEarly.comparisons == n - 1);
    assert(selection.comparisons == quadratic && selectionEarly.comparisons == 2 * (n - 1));
    assert(insertion.comparisons == n - 1 && insertionEarly.comparisons == n - 1);
    assert(insertion.copies > 0 && insertionEarly.copies == 0);
    assert(rank.comparisons == quadratic && rankEarly.comparisons == n - 1);
    assert(rankEarly.copies == 0);

    std::cout << "Comparisons on 32 sorted elements (basic -> early):\n"
              << "bubble: " << bubble.comparisons << " -> " << bubbleEarly.comparisons << '\n'
              << "selection: " << selection.comparisons << " -> " << selectionEarly.comparisons << '\n'
              << "insertion: " << insertion.comparisons << " -> " << insertionEarly.comparisons << '\n'
              << "rank: " << rank.comparisons << " -> " << rankEarly.comparisons << '\n';
}

} // namespace

int main() {
    testExhaustiveAndRandom();
    testStability();
    testHelpers();
    testEarlyExitCosts();
    std::cout << "All sorting tests passed (3280 exhaustive inputs and 200 random inputs).\n";
}
