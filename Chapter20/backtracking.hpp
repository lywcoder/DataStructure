#ifndef CHAPTER20_BACKTRACKING_HPP
#define CHAPTER20_BACKTRACKING_HPP

#include <algorithm>
#include <climits>
#include <functional>
#include <stdexcept>
#include <vector>

namespace chapter20 {

struct KnapsackItem { int weight; long long value; };
struct KnapsackResult {
    long long maxValue;
    int totalWeight;
    std::vector<int> itemIndices;
};

// 按物品编号形成二叉选择树；剩余价值总和是乐观上界。
inline KnapsackResult knapsack01(const std::vector<KnapsackItem>& items, int capacity) {
    if (capacity < 0) throw std::invalid_argument("negative capacity");
    const std::size_t n = items.size();
    std::vector<long long> suffix(n + 1, 0);
    for (std::size_t i = n; i > 0; --i) {
        if (items[i - 1].weight <= 0 || items[i - 1].value < 0)
            throw std::invalid_argument("weight must be positive and value nonnegative");
        if (suffix[i] > LLONG_MAX - items[i - 1].value)
            throw std::overflow_error("sum of values exceeds long long");
        suffix[i - 1] = suffix[i] + items[i - 1].value;
    }
    KnapsackResult best = {0, 0, {}};
    std::vector<int> chosen;
    std::function<void(std::size_t, int, long long)> dfs =
        [&](std::size_t i, int weight, long long value) {
            if (value + suffix[i] <= best.maxValue) return;
            if (i == n) {
                best.maxValue = value;
                best.totalWeight = weight;
                best.itemIndices = chosen;
                return;
            }
            if (items[i].weight <= capacity - weight) {
                chosen.push_back(static_cast<int>(i));
                dfs(i + 1, weight + items[i].weight, value + items[i].value);
                chosen.pop_back();
            }
            dfs(i + 1, weight, value);
        };
    dfs(0, 0, 0);
    return best;
}

struct CliqueResult {
    int size;
    std::vector<int> vertices;
};

// 邻接矩阵必须是无向简单图；每层决定是否选入一个顶点。
inline CliqueResult maximumClique(const std::vector<std::vector<int> >& adjacent) {
    const std::size_t n = adjacent.size();
    for (const std::vector<int>& row : adjacent)
        if (row.size() != n)
            throw std::invalid_argument("expected square simple graph matrix");
    for (std::size_t i = 0; i < n; ++i) {
        if (adjacent[i][i] != 0)
            throw std::invalid_argument("expected square simple graph matrix");
        for (std::size_t j = 0; j < n; ++j)
            if ((adjacent[i][j] != 0 && adjacent[i][j] != 1) ||
                adjacent[i][j] != adjacent[j][i])
                throw std::invalid_argument("expected symmetric 0/1 adjacency matrix");
    }
    CliqueResult best = {0, {}};
    std::vector<int> chosen;
    std::function<void(std::size_t)> dfs = [&](std::size_t i) {
        if (chosen.size() + n - i <= static_cast<std::size_t>(best.size)) return;
        if (i == n) {
            best.size = static_cast<int>(chosen.size());
            best.vertices = chosen;
            return;
        }
        bool compatible = true;
        for (int vertex : chosen)
            if (adjacent[vertex][i] == 0) { compatible = false; break; }
        if (compatible) {
            chosen.push_back(static_cast<int>(i));
            dfs(i + 1);
            chosen.pop_back();
        }
        dfs(i + 1);
    };
    dfs(0);
    return best;
}

struct TspResult {
    long long minCost; // -1 表示不存在哈密顿回路。
    std::vector<int> tour;
};

// 固定起点 0；-1 表示无边，其他边权必须非负。
inline TspResult travelingSalesperson(const std::vector<std::vector<int> >& cost) {
    const std::size_t n = cost.size();
    if (n == 0) return {-1, {}};
    std::vector<int> minOut(n, -1);
    for (std::size_t i = 0; i < n; ++i) {
        if (cost[i].size() != n || cost[i][i] != 0)
            throw std::invalid_argument("expected square matrix with zero diagonal");
        for (std::size_t j = 0; j < n; ++j) {
            if (cost[i][j] < -1)
                throw std::invalid_argument("edge cost must be nonnegative or -1");
            if (i != j && cost[i][j] >= 0 &&
                (minOut[i] < 0 || cost[i][j] < minOut[i]))
                minOut[i] = cost[i][j];
        }
    }
    if (n == 1) return {0, {0, 0}};
    for (int lower : minOut) if (lower < 0) return {-1, {}};

    TspResult best = {-1, {}};
    std::vector<int> path(1, 0);
    std::vector<bool> used(n, false);
    used[0] = true;
    std::function<void(long long)> dfs = [&](long long currentCost) {
        const int last = path.back();
        if (path.size() == n) {
            if (cost[last][0] < 0) return;
            if (currentCost > LLONG_MAX - cost[last][0])
                throw std::overflow_error("tour cost exceeds long long");
            const long long fullCost = currentCost + cost[last][0];
            if (best.minCost < 0 || fullCost < best.minCost) {
                best.minCost = fullCost;
                best.tour = path;
                best.tour.push_back(0);
            }
            return;
        }
        // 每个尚未确定出边的顶点至少要付出一次最小出边费用。
        long long lowerBound = currentCost + minOut[last];
        for (std::size_t v = 1; v < n; ++v)
            if (!used[v]) lowerBound += minOut[v];
        if (best.minCost >= 0 && lowerBound >= best.minCost) return;
        for (std::size_t v = 1; v < n; ++v) {
            if (used[v] || cost[last][v] < 0) continue;
            if (currentCost > LLONG_MAX - cost[last][v])
                throw std::overflow_error("tour cost exceeds long long");
            path.push_back(static_cast<int>(v));
            used[v] = true;
            dfs(currentCost + cost[last][v]);
            used[v] = false;
            path.pop_back();
        }
    };
    dfs(0);
    return best;
}

struct BoardResult {
    int minDensity;
    std::vector<int> order;
};

// boards[b][net] 为 1 表示板 b 有网组 net 的引脚。
inline BoardResult arrangeCircuitBoards(const std::vector<std::vector<int> >& boards) {
    const std::size_t n = boards.size();
    if (n == 0) return {0, {}};
    const std::size_t m = boards[0].size();
    std::vector<int> total(m, 0);
    for (const std::vector<int>& board : boards) {
        if (board.size() != m) throw std::invalid_argument("ragged board matrix");
        for (std::size_t net = 0; net < m; ++net) {
            if (board[net] != 0 && board[net] != 1)
                throw std::invalid_argument("board matrix must contain 0 or 1");
            total[net] += board[net];
        }
    }
    BoardResult best = {INT_MAX, {}};
    std::vector<int> placed(m, 0), order;
    std::vector<bool> used(n, false);
    std::function<void(int)> dfs = [&](int density) {
        if (order.size() == n) {
            best.minDensity = density;
            best.order = order;
            return;
        }
        for (std::size_t board = 0; board < n; ++board) {
            if (used[board]) continue;
            used[board] = true;
            order.push_back(static_cast<int>(board));
            int crossing = 0;
            for (std::size_t net = 0; net < m; ++net) {
                placed[net] += boards[board][net];
                if (placed[net] > 0 && placed[net] < total[net]) ++crossing;
            }
            const int nextDensity = std::max(density, crossing);
            if (nextDensity < best.minDensity) dfs(nextDensity);
            for (std::size_t net = 0; net < m; ++net)
                placed[net] -= boards[board][net];
            order.pop_back();
            used[board] = false;
        }
    };
    dfs(0);
    return best;
}

} // namespace chapter20

#endif
