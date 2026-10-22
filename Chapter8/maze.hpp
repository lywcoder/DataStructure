#ifndef CHAPTER8_MAZE_HPP
#define CHAPTER8_MAZE_HPP

#include "arraystack.hpp"
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

struct MazeFrame {
    int row;
    int col;
    int nextDirection; // 下次尝试的方向 0..3。
    MazeFrame(int r, int c) : row(r), col(c), nextDirection(0) {}
};

// 0 是通路、1 是墙；坐标按 C++ 数组习惯从 0 开始。
// 栈内帧始终对应当前路径；走进邻格压栈，四向都不通就弹栈回退。
inline std::vector<std::pair<int, int>> findMazePath(
    const std::vector<std::vector<int>>& maze,
    std::pair<int, int> start, std::pair<int, int> goal) {
    if (maze.empty() || maze[0].empty())
        throw std::invalid_argument("maze must be a nonempty rectangle");
    if (maze.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
        maze[0].size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::length_error("maze dimensions are too large");
    const int rows = static_cast<int>(maze.size());
    const int cols = static_cast<int>(maze[0].size());
    for (const std::vector<int>& row : maze) {
        if (static_cast<int>(row.size()) != cols)
            throw std::invalid_argument("maze rows must have equal length");
        for (int cell : row)
            if (cell != 0 && cell != 1)
                throw std::invalid_argument("maze cells must be 0 or 1");
    }
    const auto inside = [rows, cols](std::pair<int, int> p) {
        return p.first >= 0 && p.first < rows && p.second >= 0 && p.second < cols;
    };
    if (!inside(start) || !inside(goal))
        throw std::out_of_range("maze endpoint out of range");
    if (maze[static_cast<std::size_t>(start.first)][static_cast<std::size_t>(start.second)] ||
        maze[static_cast<std::size_t>(goal.first)][static_cast<std::size_t>(goal.second)])
        return {};

    const int dr[4] = {-1, 0, 1, 0};
    const int dc[4] = {0, 1, 0, -1};
    std::vector<std::vector<char>> visited(static_cast<std::size_t>(rows),
                                           std::vector<char>(static_cast<std::size_t>(cols), 0));
    arrayStack<MazeFrame> pending;
    std::vector<std::pair<int, int>> path;
    pending.push(MazeFrame(start.first, start.second));
    path.push_back(start);
    visited[static_cast<std::size_t>(start.first)][static_cast<std::size_t>(start.second)] = 1;

    while (!pending.empty()) {
        MazeFrame& current = pending.top();
        if (std::make_pair(current.row, current.col) == goal) return path;
        if (current.nextDirection == 4) {
            pending.pop();
            path.pop_back();
            continue;
        }
        const int direction = current.nextDirection++;
        const int nr = current.row + dr[direction];
        const int nc = current.col + dc[direction];
        const std::pair<int, int> next(nr, nc);
        if (!inside(next) || maze[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)] ||
            visited[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)])
            continue;
        visited[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)] = 1;
        pending.push(MazeFrame(nr, nc));
        path.push_back(next);
    }
    return {}; // 所有可达格都尝试过，目标不可达。
}

#endif
