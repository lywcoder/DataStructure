#include "divideAndConquer.hpp"

#include <iostream>
#include <vector>

void printValues(const std::vector<int>& values) {
    for (int value : values) std::cout << ' ' << value;
    std::cout << '\n';
}

int main() {
    const std::vector<std::vector<int> > board = tileDefectiveBoard(4, 1, 2);
    std::cout << "Defective board (0 = missing square):\n";
    for (const std::vector<int>& row : board) printValues(row);

    std::vector<int> mergeValues{5, 1, 3, 1, -4, 8};
    std::vector<int> quickValues = mergeValues;
    mergeSort(mergeValues);
    quickSort(quickValues);
    std::cout << "Merge sort:";
    printValues(mergeValues);
    std::cout << "Quick sort:";
    printValues(quickValues);
    std::cout << "Element at rank 3 (zero-based): "
              << selectKth({5, 1, 3, 1, -4, 8}, 3) << '\n';

    const closestPairResult pair = closestPair({{0, 0}, {9, 9}, {2, 2}, {2, 3}});
    std::cout << "Closest pair indices: " << pair.first << ", " << pair.second
              << "; distance: " << pair.distance << '\n';

    const minMaxResult extrema = findMinMax({7, 2, 9, 1, 8, 3});
    std::cout << "Min/max: " << extrema.minimum << "/" << extrema.maximum
              << "; comparisons: " << extrema.comparisons << '\n';
}
