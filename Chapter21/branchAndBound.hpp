#ifndef CHAPTER21_BRANCH_AND_BOUND_HPP
#define CHAPTER21_BRANCH_AND_BOUND_HPP

#include <algorithm>
#include <climits>
#include <cstddef>
#include <queue>
#include <stdexcept>
#include <vector>

namespace chapter21 {

struct LoadingResult {
    int maxWeight;
    std::vector<int> containerIndices;
};

// 最大优先队列：bound 是当前重量加上全部剩余货箱的重量，再受容量限制。
inline LoadingResult loadContainers(const std::vector<int>& weights, int capacity) {
    if (capacity < 0) throw std::invalid_argument("negative capacity");
    const std::size_t n = weights.size();
    std::vector<long long> suffix(n + 1, 0);
    for (std::size_t i = n; i > 0; --i) {
        if (weights[i - 1] <= 0) throw std::invalid_argument("weight must be positive");
        if (suffix[i] > LLONG_MAX - weights[i - 1])
            throw std::overflow_error("sum of weights exceeds long long");
        suffix[i - 1] = suffix[i] + weights[i - 1];
    }
    struct Node {
        std::size_t next;
        int weight;
        int bound;
        std::vector<int> selected;
    };
    const auto lowerPriority = [](const Node& a, const Node& b) {
        return a.bound < b.bound;
    };
    std::priority_queue<Node, std::vector<Node>, decltype(lowerPriority)> live(lowerPriority);
    live.push({0, 0, static_cast<int>(std::min<long long>(capacity, suffix[0])), {}});
    LoadingResult best = {0, {}};
    while (!live.empty()) {
        const Node node = live.top();
        live.pop();
        if (node.bound <= best.maxWeight) continue;
        if (node.next == n) continue;

        // 不装入：继承当前重量；装入：必须先通过容量检查。
        Node without = node;
        ++without.next;
        without.bound = static_cast<int>(std::min<long long>(
            capacity, static_cast<long long>(without.weight) + suffix[without.next]));
        if (without.bound > best.maxWeight) live.push(without);
        if (weights[node.next] <= capacity - node.weight) {
            Node with = node;
            with.weight += weights[node.next];
            with.selected.push_back(static_cast<int>(node.next));
            ++with.next;
            if (with.weight > best.maxWeight) {
                best.maxWeight = with.weight;
                best.containerIndices = with.selected;
            }
            with.bound = static_cast<int>(std::min<long long>(
                capacity, static_cast<long long>(with.weight) + suffix[with.next]));
            if (with.bound > best.maxWeight) live.push(with);
        }
    }
    return best;
}

struct TwoShipLoadingResult {
    bool feasible;
    int firstWeight;
    int secondWeight;
    std::vector<int> firstIndices;
    std::vector<int> secondIndices;
};

// 第一艘尽量装满，剩余总重量最小；若剩余不超第二艘容量即找到可行分配。
inline TwoShipLoadingResult loadTwoShips(const std::vector<int>& weights,
                                         int firstCapacity, int secondCapacity) {
    if (secondCapacity < 0) throw std::invalid_argument("negative capacity");
    const LoadingResult first = loadContainers(weights, firstCapacity);
    long long total = 0;
    for (int weight : weights) total += weight; // loadContainers 已校验总和不溢出。
    const long long remainder = total - first.maxWeight;
    if (remainder > secondCapacity) return {false, 0, 0, {}, {}};
    std::vector<bool> selected(weights.size(), false);
    for (int index : first.containerIndices) selected[index] = true;
    TwoShipLoadingResult result = {true, first.maxWeight, static_cast<int>(remainder),
                                   first.containerIndices, {}};
    for (std::size_t i = 0; i < weights.size(); ++i)
        if (!selected[i]) result.secondIndices.push_back(static_cast<int>(i));
    return result;
}

struct KnapsackItem { int weight; long long value; };
struct KnapsackResult {
    long long maxValue;
    int totalWeight;
    std::vector<int> itemIndices;
};

// 剩余价值总和忽略容量约束，因此是安全但可能较松的上界。
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
    struct Node {
        std::size_t next;
        int weight;
        long long value;
        long long bound;
        std::vector<int> selected;
    };
    const auto lowerPriority = [](const Node& a, const Node& b) {
        return a.bound < b.bound;
    };
    std::priority_queue<Node, std::vector<Node>, decltype(lowerPriority)> live(lowerPriority);
    live.push({0, 0, 0, suffix[0], {}});
    KnapsackResult best = {0, 0, {}};
    while (!live.empty()) {
        const Node node = live.top();
        live.pop();
        if (node.bound <= best.maxValue) continue;
        if (node.next == n) continue;

        Node without = node;
        ++without.next;
        without.bound = without.value + suffix[without.next];
        if (without.bound > best.maxValue) live.push(without);
        if (items[node.next].weight <= capacity - node.weight) {
            Node with = node;
            with.weight += items[node.next].weight;
            with.value += items[node.next].value;
            with.selected.push_back(static_cast<int>(node.next));
            ++with.next;
            if (with.value > best.maxValue) {
                best.maxValue = with.value;
                best.totalWeight = with.weight;
                best.itemIndices = with.selected;
            }
            with.bound = with.value + suffix[with.next];
            if (with.bound > best.maxValue) live.push(with);
        }
    }
    return best;
}

struct CliqueResult {
    int size;
    std::vector<int> vertices;
};

inline CliqueResult maximumClique(const std::vector<std::vector<int> >& adjacent) {
    const std::size_t n = adjacent.size();
    for (const std::vector<int>& row : adjacent)
        if (row.size() != n) throw std::invalid_argument("expected square graph matrix");
    for (std::size_t i = 0; i < n; ++i) {
        if (adjacent[i][i] != 0) throw std::invalid_argument("diagonal must be zero");
        for (std::size_t j = 0; j < n; ++j)
            if ((adjacent[i][j] != 0 && adjacent[i][j] != 1) ||
                adjacent[i][j] != adjacent[j][i])
                throw std::invalid_argument("expected symmetric 0/1 matrix");
    }
    struct Node {
        std::size_t next;
        int bound;
        std::vector<int> selected;
    };
    const auto lowerPriority = [](const Node& a, const Node& b) {
        return a.bound < b.bound;
    };
    std::priority_queue<Node, std::vector<Node>, decltype(lowerPriority)> live(lowerPriority);
    live.push({0, static_cast<int>(n), {}});
    CliqueResult best = {0, {}};
    while (!live.empty()) {
        const Node node = live.top();
        live.pop();
        if (node.bound <= best.size || node.next == n) continue;
        Node without = node;
        ++without.next;
        without.bound = static_cast<int>(without.selected.size() + n - without.next);
        if (without.bound > best.size) live.push(without);

        bool compatible = true;
        for (int vertex : node.selected)
            if (adjacent[vertex][node.next] == 0) { compatible = false; break; }
        if (compatible) {
            Node with = node;
            with.selected.push_back(static_cast<int>(node.next));
            ++with.next;
            if (static_cast<int>(with.selected.size()) > best.size) {
                best.size = static_cast<int>(with.selected.size());
                best.vertices = with.selected;
            }
            with.bound = static_cast<int>(with.selected.size() + n - with.next);
            if (with.bound > best.size) live.push(with);
        }
    }
    return best;
}

struct TspResult {
    long long minCost; // -1 表示不存在回路。
    std::vector<int> tour;
};

inline long long addNonnegative(long long a, long long b) {
    if (a > LLONG_MAX - b) throw std::overflow_error("tour cost exceeds long long");
    return a + b;
}

// 固定起点 0；每个未确定出边的顶点至少付出它的最小出边费用。
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
                (minOut[i] < 0 || cost[i][j] < minOut[i])) minOut[i] = cost[i][j];
        }
    }
    if (n == 1) return {0, {0, 0}};
    for (int edge : minOut) if (edge < 0) return {-1, {}};
    struct Node {
        long long costSoFar;
        long long bound;
        std::vector<int> path;
        std::vector<bool> used;
    };
    const auto higherPriority = [](const Node& a, const Node& b) {
        return a.bound > b.bound;
    };
    std::priority_queue<Node, std::vector<Node>, decltype(higherPriority)> live(higherPriority);
    Node root = {0, 0, {0}, std::vector<bool>(n, false)};
    root.used[0] = true;
    root.bound = minOut[0];
    for (std::size_t v = 1; v < n; ++v) root.bound = addNonnegative(root.bound, minOut[v]);
    live.push(root);
    TspResult best = {-1, {}};
    while (!live.empty()) {
        const Node node = live.top();
        live.pop();
        if (best.minCost >= 0 && node.bound >= best.minCost) continue;
        const int last = node.path.back();
        for (std::size_t v = 1; v < n; ++v) {
            if (node.used[v] || cost[last][v] < 0) continue;
            Node child = node;
            child.costSoFar = addNonnegative(node.costSoFar, cost[last][v]);
            child.path.push_back(static_cast<int>(v));
            child.used[v] = true;
            if (child.path.size() == n) {
                if (cost[v][0] < 0) continue;
                const long long total = addNonnegative(child.costSoFar, cost[v][0]);
                if (best.minCost < 0 || total < best.minCost) {
                    best.minCost = total;
                    best.tour = child.path;
                    best.tour.push_back(0);
                }
                continue;
            }
            child.bound = addNonnegative(child.costSoFar, minOut[v]);
            for (std::size_t u = 1; u < n; ++u)
                if (!child.used[u]) child.bound = addNonnegative(child.bound, minOut[u]);
            if (best.minCost < 0 || child.bound < best.minCost) live.push(child);
        }
    }
    return best;
}

struct BoardResult {
    int minDensity;
    std::vector<int> order;
};

// 当前前缀已出现的最大切口密度是最终目标的下界。
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
    struct Node {
        int bound;
        std::vector<int> order;
        std::vector<bool> used;
        std::vector<int> placed;
    };
    const auto higherPriority = [](const Node& a, const Node& b) {
        return a.bound > b.bound;
    };
    std::priority_queue<Node, std::vector<Node>, decltype(higherPriority)> live(higherPriority);
    live.push({0, {}, std::vector<bool>(n, false), std::vector<int>(m, 0)});
    BoardResult best = {INT_MAX, {}};
    while (!live.empty()) {
        const Node node = live.top();
        live.pop();
        if (node.bound >= best.minDensity) continue;
        for (std::size_t board = 0; board < n; ++board) {
            if (node.used[board]) continue;
            Node child = node;
            child.order.push_back(static_cast<int>(board));
            child.used[board] = true;
            int crossing = 0;
            for (std::size_t net = 0; net < m; ++net) {
                child.placed[net] += boards[board][net];
                if (child.placed[net] > 0 && child.placed[net] < total[net]) ++crossing;
            }
            child.bound = std::max(node.bound, crossing);
            if (child.order.size() == n) {
                if (child.bound < best.minDensity) {
                    best.minDensity = child.bound;
                    best.order = child.order;
                }
            } else if (child.bound < best.minDensity) live.push(child);
        }
    }
    return best;
}

} // namespace chapter21

#endif
