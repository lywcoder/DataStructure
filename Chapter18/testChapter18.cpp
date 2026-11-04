#include "divideAndConquer.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

void testDefectiveBoard() {
    const std::vector<std::vector<int> > board = tileDefectiveBoard(4, 1, 2);
    assert(board.size() == 4 && board[1][2] == 0);
    std::vector<int> counts(6, 0);
    for (const std::vector<int>& row : board)
        for (int id : row) {
            assert(id >= 0 && id <= 5);
            ++counts[id];
        }
    assert(counts[0] == 1);
    for (int id = 1; id <= 5; ++id) assert(counts[id] == 3);
    for (int id = 1; id <= 5; ++id) {
        int lowRow = 4, highRow = -1, lowCol = 4, highCol = -1;
        for (int row = 0; row < 4; ++row)
            for (int col = 0; col < 4; ++col)
                if (board[row][col] == id) {
                    lowRow = std::min(lowRow, row);
                    highRow = std::max(highRow, row);
                    lowCol = std::min(lowCol, col);
                    highCol = std::max(highCol, col);
                }
        assert(highRow - lowRow == 1 && highCol - lowCol == 1);
    }
    assert((tileDefectiveBoard(1, 0, 0) == std::vector<std::vector<int> >{{0}}));
    bool invalid = false;
    try { tileDefectiveBoard(3, 0, 0); }
    catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
    bool outside = false;
    try { tileDefectiveBoard(4, 4, 0); }
    catch (const std::out_of_range&) { outside = true; }
    assert(outside);
}

void testSortingAndSelection() {
    std::vector<int> merge{5, 1, 3, 1, -4, 8};
    mergeSort(merge);
    assert((merge == std::vector<int>{-4, 1, 1, 3, 5, 8}));
    std::vector<int> quick{5, 1, 3, 1, -4, 8};
    quickSort(quick);
    assert(quick == merge);
    std::vector<int> equal(100, 7);
    quickSort(equal);
    assert(equal.front() == 7 && equal.back() == 7);
    assert(selectKth({5, 1, 3, 1, -4, 8}, 0) == -4);
    assert(selectKth({5, 1, 3, 1, -4, 8}, 3) == 3);
    assert(selectKth({5, 1, 3, 1, -4, 8}, 5) == 8);
    bool invalid = false;
    try { selectKth({}, 0); }
    catch (const std::out_of_range&) { invalid = true; }
    assert(invalid);
    unsigned int seed = 1234567;
    for (int n = 1; n <= 40; ++n) {
        std::vector<int> data;
        for (int i = 0; i < n; ++i) {
            seed = seed * 1664525u + 1013904223u;
            data.push_back(static_cast<int>(seed % 29u) - 14);
        }
        std::vector<int> expected = data;
        std::sort(expected.begin(), expected.end());
        std::vector<int> actual = data;
        mergeSort(actual);
        assert(actual == expected);
        actual = data;
        quickSort(actual);
        assert(actual == expected);
        for (int k = 0; k < n; ++k)
            assert(selectKth(data, static_cast<std::size_t>(k)) == expected[k]);
    }
}

void testClosestPair() {
    const std::vector<point2D> points{{0, 0}, {9, 9}, {2, 2}, {2, 3}, {20, 1}};
    const closestPairResult result = closestPair(points);
    assert(result.first == 2 && result.second == 3);
    assert(result.distance == 1.0);
    const closestPairResult cross = closestPair({{-1, 0}, {1, 0}, {0, 0.2}, {10, 10}});
    assert(std::fabs(cross.distance - std::sqrt(1.04)) < 1e-12);
    assert(closestPair({{3, 4}, {3, 4}}).distance == 0.0);
    assert(closestPair({{0, 0}, {1e200, 0}}).distance == 1e200);
    bool invalid = false;
    try { closestPair({{0, 0}}); }
    catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
    invalid = false;
    try { closestPair({{0, 0}, {std::numeric_limits<double>::infinity(), 0}}); }
    catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
    for (int n = 2; n <= 25; ++n) {
        std::vector<point2D> sample;
        for (int i = 0; i < n; ++i)
            sample.push_back(point2D{static_cast<double>((i * 17) % 23),
                                     static_cast<double>((i * i * 7) % 31)});
        double brute = std::numeric_limits<double>::infinity();
        for (int i = 0; i < n; ++i)
            for (int j = i + 1; j < n; ++j)
                brute = std::min(brute, std::hypot(sample[i].x - sample[j].x,
                                                   sample[i].y - sample[j].y));
        assert(std::fabs(closestPair(sample).distance - brute) < 1e-12);
    }
}

void testMinMax() {
    const minMaxResult even = findMinMax({7, 2, 9, 1, 8, 3});
    assert(even.minimum == 1 && even.maximum == 9 && even.comparisons == 7);
    const minMaxResult odd = findMinMax({7, 2, 9, 1, 8});
    assert(odd.minimum == 1 && odd.maximum == 9 && odd.comparisons == 6);
    assert(findMinMax({42}).comparisons == 0);
    bool invalid = false;
    try { findMinMax({}); }
    catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
}

int main() {
    testDefectiveBoard();
    testSortingAndSelection();
    testClosestPair();
    testMinMax();
}
