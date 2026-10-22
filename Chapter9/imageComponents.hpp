#ifndef CHAPTER9_IMAGE_COMPONENTS_HPP
#define CHAPTER9_IMAGE_COMPONENTS_HPP

#include "grid.hpp"
#include "linkedQueue.hpp"
#include <vector>

struct ComponentLabeling {
    std::vector<std::vector<int>> labels; // 背景为 0，图元编号从 1 开始。
    std::vector<int> sizes;               // sizes[id-1] 是该图元的像素数。
    int count() const { return static_cast<int>(sizes.size()); }
};

// 从每个尚未标号的前景像素出发，用队列扩展整个连通图元。
inline ComponentLabeling recognizeComponents(
    const std::vector<std::vector<int>>& image, bool eightConnected = false) {
    const std::pair<int, int> dims = checkedBinaryGrid(image, true);
    const int rows = dims.first, cols = dims.second;
    ComponentLabeling result;
    result.labels.assign(static_cast<std::size_t>(rows),
        std::vector<int>(static_cast<std::size_t>(cols), 0));
    const int dr[8] = {-1, 0, 1, 0, -1, -1, 1, 1};
    const int dc[8] = {0, 1, 0, -1, -1, 1, -1, 1};
    const int directionCount = eightConnected ? 8 : 4;
    for (int row = 0; row < rows; ++row)
        for (int col = 0; col < cols; ++col) {
            if (image[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] == 0 ||
                result.labels[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] != 0)
                continue;
            const int id = result.count() + 1;
            int componentSize = 0;
            linkedQueue<GridPoint> pending;
            pending.push(GridPoint(row, col));
            result.labels[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] = id;
            while (!pending.empty()) {
                const GridPoint current = pending.front();
                pending.pop();
                ++componentSize;
                for (int d = 0; d < directionCount; ++d) {
                    const GridPoint next(current.row + dr[d], current.col + dc[d]);
                    if (!insideGrid(next, rows, cols) ||
                        image[static_cast<std::size_t>(next.row)][static_cast<std::size_t>(next.col)] == 0 ||
                        result.labels[static_cast<std::size_t>(next.row)][static_cast<std::size_t>(next.col)] != 0)
                        continue;
                    result.labels[static_cast<std::size_t>(next.row)][static_cast<std::size_t>(next.col)] = id;
                    pending.push(next); // 入队时标记，避免同一像素被重复入队。
                }
            }
            result.sizes.push_back(componentSize);
        }
    return result;
}

#endif
