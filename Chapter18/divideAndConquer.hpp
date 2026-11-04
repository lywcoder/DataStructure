#ifndef CHAPTER18_DIVIDE_AND_CONQUER_HPP
#define CHAPTER18_DIVIDE_AND_CONQUER_HPP

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

// 0 是残缺方格；同一正整数的三个方格构成一块 L 形骨牌。
inline void tileBoardRegion(std::vector<std::vector<int> >& board, int row, int col,
                           int size, int missingRow, int missingCol, int& nextId) {
    if (size == 1) return;
    const int half = size / 2;
    const int middleRow = row + half;
    const int middleCol = col + half;
    const int missingQuadrant = (missingRow >= middleRow ? 2 : 0) +
                                (missingCol >= middleCol ? 1 : 0);
    const int centerRows[4] = {middleRow - 1, middleRow - 1, middleRow, middleRow};
    const int centerCols[4] = {middleCol - 1, middleCol, middleCol - 1, middleCol};
    const int id = nextId++;
    for (int q = 0; q < 4; ++q)
        if (q != missingQuadrant) board[centerRows[q]][centerCols[q]] = id;
    for (int q = 0; q < 4; ++q) {
        const int subRow = row + (q / 2) * half;
        const int subCol = col + (q % 2) * half;
        tileBoardRegion(board, subRow, subCol, half,
                        q == missingQuadrant ? missingRow : centerRows[q],
                        q == missingQuadrant ? missingCol : centerCols[q], nextId);
    }
}

inline std::vector<std::vector<int> > tileDefectiveBoard(int size, int missingRow,
                                                          int missingCol) {
    if (size <= 0 || (size & (size - 1)) != 0)
        throw std::invalid_argument("board size must be a positive power of two");
    if (missingRow < 0 || missingRow >= size || missingCol < 0 || missingCol >= size)
        throw std::out_of_range("missing square is outside the board");
    std::vector<std::vector<int> > board(size, std::vector<int>(size, 0));
    int nextId = 1;
    tileBoardRegion(board, 0, 0, size, missingRow, missingCol, nextId);
    return board;
}

inline void mergeSortRange(std::vector<int>& values, std::vector<int>& buffer,
                           std::size_t begin, std::size_t end) {
    if (end - begin < 2) return;
    const std::size_t middle = begin + (end - begin) / 2;
    mergeSortRange(values, buffer, begin, middle);
    mergeSortRange(values, buffer, middle, end);
    std::size_t i = begin, j = middle;
    for (std::size_t at = begin; at < end; ++at) {
        if (j == end || (i < middle && values[i] <= values[j])) buffer[at] = values[i++];
        else buffer[at] = values[j++];
    }
    std::copy(buffer.begin() + begin, buffer.begin() + end, values.begin() + begin);
}

inline void mergeSort(std::vector<int>& values) {
    std::vector<int> buffer(values.size());
    mergeSortRange(values, buffer, 0, values.size());
}

// 返回 [begin, lessEnd) < pivot、[lessEnd, greaterBegin) == pivot。
inline std::pair<std::size_t, std::size_t> partitionThreeWay(
    std::vector<int>& values, std::size_t begin, std::size_t end) {
    const int pivot = values[begin + (end - begin) / 2];
    std::size_t lessEnd = begin, current = begin, greaterBegin = end;
    while (current < greaterBegin) {
        if (values[current] < pivot) std::swap(values[lessEnd++], values[current++]);
        else if (values[current] > pivot) std::swap(values[current], values[--greaterBegin]);
        else ++current;
    }
    return std::make_pair(lessEnd, greaterBegin);
}

inline void quickSortRange(std::vector<int>& values, std::size_t begin, std::size_t end) {
    while (end - begin > 1) {
        const std::pair<std::size_t, std::size_t> equal = partitionThreeWay(values, begin, end);
        // 先递归较短一侧，使最坏情况的调用栈也只有 O(log n)。
        if (equal.first - begin < end - equal.second) {
            quickSortRange(values, begin, equal.first);
            begin = equal.second;
        } else {
            quickSortRange(values, equal.second, end);
            end = equal.first;
        }
    }
}

inline void quickSort(std::vector<int>& values) {
    quickSortRange(values, 0, values.size());
}

// k 从 0 起计数；输入按值传递，避免破坏调用者的序列。
inline int selectKth(std::vector<int> values, std::size_t k) {
    if (k >= values.size()) throw std::out_of_range("selection rank is outside the array");
    std::size_t begin = 0, end = values.size();
    while (true) {
        const std::pair<std::size_t, std::size_t> equal = partitionThreeWay(values, begin, end);
        if (k < equal.first) end = equal.first;
        else if (k >= equal.second) begin = equal.second;
        else return values[k];
    }
}

struct point2D { double x; double y; };
struct closestPairResult { std::size_t first; std::size_t second; double distance; };

struct indexedPoint { double x; double y; std::size_t index; };
struct pointPairCandidate { double distance; std::size_t first; std::size_t second; };

inline void considerPointPair(const indexedPoint& a, const indexedPoint& b,
                             pointPairCandidate& best) {
    const double distance = std::hypot(a.x - b.x, a.y - b.y);
    if (distance < best.distance) {
        best.distance = distance;
        best.first = std::min(a.index, b.index);
        best.second = std::max(a.index, b.index);
    }
}

inline pointPairCandidate closestPairRange(std::vector<indexedPoint>& points,
                                          std::vector<indexedPoint>& buffer,
                                          std::size_t begin, std::size_t end) {
    pointPairCandidate best = {std::numeric_limits<double>::infinity(), 0, 0};
    if (end - begin <= 3) {
        for (std::size_t i = begin; i < end; ++i)
            for (std::size_t j = i + 1; j < end; ++j)
                considerPointPair(points[i], points[j], best);
        std::sort(points.begin() + begin, points.begin() + end,
                  [](const indexedPoint& a, const indexedPoint& b) { return a.y < b.y; });
        return best;
    }
    const std::size_t middle = begin + (end - begin) / 2;
    const double middleX = points[middle].x;
    const pointPairCandidate left = closestPairRange(points, buffer, begin, middle);
    const pointPairCandidate right = closestPairRange(points, buffer, middle, end);
    best = left.distance <= right.distance ? left : right;
    std::merge(points.begin() + begin, points.begin() + middle,
               points.begin() + middle, points.begin() + end, buffer.begin() + begin,
               [](const indexedPoint& a, const indexedPoint& b) { return a.y < b.y; });
    std::copy(buffer.begin() + begin, buffer.begin() + end, points.begin() + begin);
    std::vector<indexedPoint> strip;
    strip.reserve(end - begin);
    for (std::size_t i = begin; i < end; ++i) {
        const double dx = points[i].x - middleX;
        if (std::fabs(dx) > best.distance) continue;
        // y 有序；平面装箱论证表明每点只须检查随后至多 7 点。
        for (std::size_t j = strip.size(); j > 0 && strip.size() - j < 7; --j) {
            const double dy = points[i].y - strip[j - 1].y;
            if (dy > best.distance) break;
            considerPointPair(points[i], strip[j - 1], best);
        }
        strip.push_back(points[i]);
    }
    return best;
}

inline closestPairResult closestPair(const std::vector<point2D>& input) {
    if (input.size() < 2) throw std::invalid_argument("at least two points are required");
    std::vector<indexedPoint> points;
    points.reserve(input.size());
    for (std::size_t i = 0; i < input.size(); ++i) {
        if (!std::isfinite(input[i].x) || !std::isfinite(input[i].y))
            throw std::invalid_argument("point coordinates must be finite");
        points.push_back(indexedPoint{input[i].x, input[i].y, i});
    }
    std::sort(points.begin(), points.end(), [](const indexedPoint& a, const indexedPoint& b) {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    });
    std::vector<indexedPoint> buffer(points.size());
    const pointPairCandidate best = closestPairRange(points, buffer, 0, points.size());
    if (!std::isfinite(best.distance))
        throw std::overflow_error("closest distance cannot be represented as double");
    return {best.first, best.second, best.distance};
}

struct minMaxResult { int minimum; int maximum; std::size_t comparisons; };

inline minMaxResult findMinMax(const std::vector<int>& values) {
    if (values.empty()) throw std::invalid_argument("min/max needs a nonempty array");
    minMaxResult result = {values[0], values[0], 0};
    std::size_t at = 1;
    if (values.size() % 2 == 0) {
        ++result.comparisons;
        if (values[0] < values[1]) result.maximum = values[1];
        else result.minimum = values[1];
        at = 2;
    }
    for (; at + 1 < values.size(); at += 2) {
        ++result.comparisons;
        const bool ordered = values[at] < values[at + 1];
        const int small = ordered ? values[at] : values[at + 1];
        const int large = ordered ? values[at + 1] : values[at];
        if (small < result.minimum) result.minimum = small;
        if (large > result.maximum) result.maximum = large;
        result.comparisons += 2;
    }
    return result;
}

#endif
