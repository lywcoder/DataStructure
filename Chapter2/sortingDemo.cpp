#include "sortingAlgorithms.hpp"
#include <iostream>
#include <vector>

namespace {

void print(const std::vector<int>& values) {
    for (int value : values)
        std::cout << value << ' ';
    std::cout << '\n';
}

struct Algorithm {
    const char* name;
    void (*sort)(int[], int);
};

} // namespace

int main() {
    const Algorithm algorithms[] = {
        {"Bubble", chapter2::bubbleSort<int>},
        {"Bubble early exit", chapter2::bubbleSortEarlyExit<int>},
        {"Selection", chapter2::selectionSort<int>},
        {"Selection early exit", chapter2::selectionSortEarlyExit<int>},
        {"Insertion", chapter2::insertionSort<int>},
        {"Insertion skip ordered element", chapter2::insertionSortEarlyExit<int>},
        {"Rank", chapter2::rankSort<int>},
        {"Rank with ordered-input check", chapter2::rankSortEarlyExit<int>}
    };
    std::vector<int> input = {3, 1, 4, 1, 5, 2};
    std::cout << "Input: ";
    print(input);
    for (const Algorithm& algorithm : algorithms) {
        std::vector<int> values(input);
        algorithm.sort(values.data(), static_cast<int>(values.size()));
        std::cout << algorithm.name << ": ";
        print(values);
    }

    std::vector<int> ranks(input.size());
    chapter2::rank(input.data(), static_cast<int>(input.size()), ranks.data());
    std::cout << "Ranks in original order: ";
    print(ranks);

    // 不能把“选择排序没有交换”当作整个区间有序。
    std::vector<int> selectionCounterexample = {3, 1, 2, 4};
    chapter2::selectionSortEarlyExit(selectionCounterexample.data(), 4);
    std::cout << "Selection counterexample sorted: ";
    print(selectionCounterexample);

    // 一次插入没有移动，不代表后面的元素也已有序。
    std::vector<int> insertionCounterexample = {1, 2, 4, 3};
    chapter2::insertionSortEarlyExit(insertionCounterexample.data(), 4);
    std::cout << "Insertion counterexample sorted: ";
    print(insertionCounterexample);
}
