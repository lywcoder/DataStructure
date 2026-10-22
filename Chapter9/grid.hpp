#ifndef CHAPTER9_GRID_HPP
#define CHAPTER9_GRID_HPP

#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

struct GridPoint {
    int row;
    int col;
    GridPoint(int r = -1, int c = -1) : row(r), col(c) {}
    bool operator==(const GridPoint& other) const {
        return row == other.row && col == other.col;
    }
};
inline std::ostream& operator<<(std::ostream& out, const GridPoint& p) {
    return out << '(' << p.row << ',' << p.col << ')';
}

// 图元与布线都使用矩形 0/1 网格；允许空网格仅用于图元识别。
inline std::pair<int, int> checkedBinaryGrid(
    const std::vector<std::vector<int>>& grid, bool allowEmpty) {
    if (grid.empty()) {
        if (allowEmpty) return {0, 0};
        throw std::invalid_argument("grid must be nonempty");
    }
    if (grid[0].empty()) throw std::invalid_argument("grid rows must be nonempty");
    if (grid.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
        grid[0].size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::length_error("grid dimensions are too large");
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());
    for (const std::vector<int>& row : grid) {
        if (static_cast<int>(row.size()) != cols)
            throw std::invalid_argument("grid must be rectangular");
        for (int value : row)
            if (value != 0 && value != 1)
                throw std::invalid_argument("grid cells must be 0 or 1");
    }
    return {rows, cols};
}
inline bool insideGrid(GridPoint p, int rows, int cols) {
    return p.row >= 0 && p.row < rows && p.col >= 0 && p.col < cols;
}

#endif
