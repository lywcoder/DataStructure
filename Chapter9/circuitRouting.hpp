#ifndef CHAPTER9_CIRCUIT_ROUTING_HPP
#define CHAPTER9_CIRCUIT_ROUTING_HPP

#include "arrayQueue.hpp"
#include "grid.hpp"
#include <algorithm>
#include <vector>

// Lee 波前算法：0 可走、1 障碍；四邻接的每一步长度相同。
// BFS 首次到达终点时，parent 链给出最少步数的路径。
inline std::vector<GridPoint> routeCircuit(
    const std::vector<std::vector<int>>& board, GridPoint source, GridPoint target) {
    const std::pair<int, int> dims = checkedBinaryGrid(board, false);
    const int rows = dims.first, cols = dims.second;
    if (!insideGrid(source, rows, cols) || !insideGrid(target, rows, cols))
        throw std::out_of_range("routing endpoint outside board");
    if (board[static_cast<std::size_t>(source.row)][static_cast<std::size_t>(source.col)] ||
        board[static_cast<std::size_t>(target.row)][static_cast<std::size_t>(target.col)])
        return {};
    const int dr[4] = {-1, 0, 1, 0};
    const int dc[4] = {0, 1, 0, -1};
    std::vector<std::vector<char>> seen(static_cast<std::size_t>(rows),
        std::vector<char>(static_cast<std::size_t>(cols), 0));
    std::vector<std::vector<GridPoint>> parent(static_cast<std::size_t>(rows),
        std::vector<GridPoint>(static_cast<std::size_t>(cols)));
    arrayQueue<GridPoint> pending;
    pending.push(source);
    seen[static_cast<std::size_t>(source.row)][static_cast<std::size_t>(source.col)] = 1;
    bool found = source == target;
    while (!pending.empty() && !found) {
        const GridPoint current = pending.front();
        pending.pop();
        for (int d = 0; d < 4; ++d) {
            const GridPoint next(current.row + dr[d], current.col + dc[d]);
            if (!insideGrid(next, rows, cols) ||
                board[static_cast<std::size_t>(next.row)][static_cast<std::size_t>(next.col)] ||
                seen[static_cast<std::size_t>(next.row)][static_cast<std::size_t>(next.col)])
                continue;
            seen[static_cast<std::size_t>(next.row)][static_cast<std::size_t>(next.col)] = 1;
            parent[static_cast<std::size_t>(next.row)][static_cast<std::size_t>(next.col)] = current;
            if (next == target) { found = true; break; }
            pending.push(next);
        }
    }
    if (!found) return {};
    std::vector<GridPoint> path;
    for (GridPoint p = target; !(p == source);
         p = parent[static_cast<std::size_t>(p.row)][static_cast<std::size_t>(p.col)])
        path.push_back(p);
    path.push_back(source);
    std::reverse(path.begin(), path.end());
    return path;
}

#endif
