#ifndef CHAPTER19_DYNAMIC_PROGRAMMING_HPP
#define CHAPTER19_DYNAMIC_PROGRAMMING_HPP

#include <algorithm>
#include <climits>
#include <stdexcept>
#include <string>
#include <vector>

namespace chapter19 {

// 图算法用 infinity 表示不可达；有限路径权值必须保持在 (-infinity, infinity) 内。
const long long infinity = LLONG_MAX / 4;

inline long long addFinite(long long a, long long b) {
    if (b > 0 && a >= infinity - b)
        throw std::overflow_error("path distance exceeds finite range");
    if (b < 0 && a <= -infinity - b)
        throw std::overflow_error("path distance exceeds finite range");
    return a + b;
}

struct KnapsackItem { int weight; long long value; };
struct KnapsackResult {
    long long maxValue;
    int totalWeight;
    std::vector<int> itemIndices;
};

// dp[i][c]：前 i 件物品、容量 c 的最大价值；逆推时价值平局约定不选。
inline KnapsackResult knapsack01(const std::vector<KnapsackItem>& items, int capacity) {
    if (capacity < 0) throw std::invalid_argument("negative capacity");
    for (const KnapsackItem& item : items)
        if (item.weight <= 0 || item.value < 0)
            throw std::invalid_argument("weight must be positive and value nonnegative");

    const std::size_t n = items.size();
    std::vector<std::vector<long long> > dp(n + 1,
        std::vector<long long>(static_cast<std::size_t>(capacity) + 1, 0));
    for (std::size_t i = 1; i <= n; ++i) {
        for (int c = 0; c <= capacity; ++c) {
            dp[i][c] = dp[i - 1][c];
            if (items[i - 1].weight <= c) {
                const long long previous = dp[i - 1][c - items[i - 1].weight];
                if (previous > LLONG_MAX - items[i - 1].value)
                    throw std::overflow_error("knapsack value overflow");
                const long long take = previous + items[i - 1].value;
                if (take > dp[i][c]) dp[i][c] = take;
            }
        }
    }
    KnapsackResult result = {dp[n][capacity], 0, {}};
    int c = capacity;
    for (std::size_t i = n; i > 0; --i) {
        if (dp[i][c] == dp[i - 1][c]) continue;
        result.itemIndices.push_back(static_cast<int>(i - 1));
        result.totalWeight += items[i - 1].weight;
        c -= items[i - 1].weight;
    }
    std::reverse(result.itemIndices.begin(), result.itemIndices.end());
    return result;
}

struct MatrixChainResult {
    long long minCost;
    std::string parenthesization;
};

inline std::string matrixOrderText(
    const std::vector<std::vector<int> >& split, int left, int right) {
    if (left == right) return "A" + std::to_string(left + 1);
    const int middle = split[left][right];
    return "(" + matrixOrderText(split, left, middle) + "*" +
        matrixOrderText(split, middle + 1, right) + ")";
}

// dimensions 为 p[0..n]，矩阵 Ai 的维度是 p[i-1] × p[i]。
inline MatrixChainResult matrixChainOrder(const std::vector<int>& dimensions) {
    if (dimensions.size() < 2) throw std::invalid_argument("at least one matrix required");
    for (int dimension : dimensions)
        if (dimension <= 0) throw std::invalid_argument("matrix dimensions must be positive");
    const int n = static_cast<int>(dimensions.size()) - 1;
    std::vector<std::vector<long long> > cost(n, std::vector<long long>(n, 0));
    std::vector<std::vector<int> > split(n, std::vector<int>(n, 0));
    for (int length = 2; length <= n; ++length) {
        for (int left = 0; left + length <= n; ++left) {
            const int right = left + length - 1;
            cost[left][right] = LLONG_MAX;
            for (int middle = left; middle < right; ++middle) {
                const long long a = dimensions[left];
                const long long b = dimensions[middle + 1];
                const long long c = dimensions[right + 1];
                if (a > LLONG_MAX / b || a * b > LLONG_MAX / c)
                    throw std::overflow_error("matrix multiplication count overflow");
                const long long multiply = a * b * c;
                if (cost[left][middle] > LLONG_MAX - cost[middle + 1][right] ||
                    cost[left][middle] + cost[middle + 1][right] > LLONG_MAX - multiply)
                    throw std::overflow_error("matrix multiplication count overflow");
                const long long candidate = cost[left][middle] + cost[middle + 1][right] + multiply;
                if (candidate < cost[left][right]) {
                    cost[left][right] = candidate;
                    split[left][right] = middle;
                }
            }
        }
    }
    return {cost[0][n - 1], matrixOrderText(split, 0, n - 1)};
}

struct FloydWarshallResult {
    std::vector<std::vector<long long> > distance;
    std::vector<std::vector<int> > next;
};

// 权重矩阵的 infinity 表示无边；检测到任意负环时拒绝返回最短路径。
inline FloydWarshallResult floydWarshall(const std::vector<std::vector<long long> >& weights) {
    const std::size_t n = weights.size();
    FloydWarshallResult result = {weights, std::vector<std::vector<int> >(n, std::vector<int>(n, -1))};
    for (std::size_t i = 0; i < n; ++i) {
        if (weights[i].size() != n) throw std::invalid_argument("weight matrix must be square");
        for (std::size_t j = 0; j < n; ++j) {
            if (weights[i][j] != infinity) {
                if (weights[i][j] <= -infinity || weights[i][j] >= infinity)
                    throw std::invalid_argument("edge weight out of finite range");
                result.next[i][j] = static_cast<int>(j);
            }
        }
        if (weights[i][i] != 0)
            throw std::invalid_argument("weight matrix diagonal must be zero");
    }
    for (std::size_t k = 0; k < n; ++k) {
        for (std::size_t i = 0; i < n; ++i) {
            if (result.distance[i][k] == infinity) continue;
            for (std::size_t j = 0; j < n; ++j) {
                if (result.distance[k][j] == infinity) continue;
                const long long through = addFinite(result.distance[i][k], result.distance[k][j]);
                if (through < result.distance[i][j]) {
                    result.distance[i][j] = through;
                    result.next[i][j] = result.next[i][k];
                }
            }
        }
    }
    for (std::size_t v = 0; v < n; ++v)
        if (result.distance[v][v] < 0)
            throw std::invalid_argument("graph contains a negative cycle");
    return result;
}

inline std::vector<int> restoreFloydPath(
    const FloydWarshallResult& result, int source, int target) {
    const int n = static_cast<int>(result.next.size());
    if (source < 0 || source >= n || target < 0 || target >= n)
        throw std::out_of_range("invalid vertex");
    if (result.next[source][target] == -1) return {};
    std::vector<int> path(1, source);
    while (source != target) {
        source = result.next[source][target];
        path.push_back(source);
    }
    return path;
}

struct WeightedEdge { int from; int to; long long weight; };
struct BellmanFordResult {
    std::vector<long long> distance;
    std::vector<int> parent;
};

// 第 r 轮松弛后得到至多 r 条边的最短简单路径；第 V 轮仍改进意味着可达负环。
inline BellmanFordResult bellmanFord(
    int vertexCount, const std::vector<WeightedEdge>& edges, int source) {
    if (vertexCount <= 0) throw std::invalid_argument("vertex count must be positive");
    if (source < 0 || source >= vertexCount) throw std::out_of_range("invalid source");
    for (const WeightedEdge& edge : edges) {
        if (edge.from < 0 || edge.from >= vertexCount ||
            edge.to < 0 || edge.to >= vertexCount)
            throw std::out_of_range("invalid edge vertex");
        if (edge.weight <= -infinity || edge.weight >= infinity)
            throw std::invalid_argument("edge weight out of finite range");
    }
    BellmanFordResult result = {
        std::vector<long long>(vertexCount, infinity), std::vector<int>(vertexCount, -1)
    };
    result.distance[source] = 0;
    for (int pass = 1; pass < vertexCount; ++pass) {
        bool changed = false;
        for (const WeightedEdge& edge : edges) {
            if (result.distance[edge.from] == infinity) continue;
            const long long candidate = addFinite(result.distance[edge.from], edge.weight);
            if (candidate < result.distance[edge.to]) {
                result.distance[edge.to] = candidate;
                result.parent[edge.to] = edge.from;
                changed = true;
            }
        }
        if (!changed) break;
    }
    for (const WeightedEdge& edge : edges) {
        if (result.distance[edge.from] != infinity &&
            addFinite(result.distance[edge.from], edge.weight) < result.distance[edge.to])
            throw std::invalid_argument("reachable negative cycle");
    }
    return result;
}

inline std::vector<int> restoreBellmanFordPath(
    const BellmanFordResult& result, int source, int target) {
    const int n = static_cast<int>(result.distance.size());
    if (source < 0 || source >= n || target < 0 || target >= n)
        throw std::out_of_range("invalid vertex");
    if (result.distance[target] == infinity) return {};
    std::vector<int> reversed;
    for (int v = target; v != -1; v = result.parent[v]) {
        reversed.push_back(v);
        if (v == source) break;
    }
    if (reversed.back() != source) return {};
    std::reverse(reversed.begin(), reversed.end());
    return reversed;
}

struct NoncrossingNetsResult {
    int count;
    std::vector<int> upperIndices;
};

// 上排从左至右编号 0..n-1；lowerEndOfUpper[i] 是线 i 对应的下排端点位置。
// 无交叉等价于选出的下排端点序列严格递增，因此可用最长递增子序列 DP。
inline NoncrossingNetsResult maximumNoncrossingNets(
    const std::vector<int>& lowerEndOfUpper) {
    const int n = static_cast<int>(lowerEndOfUpper.size());
    std::vector<bool> seen(n, false);
    for (int lower : lowerEndOfUpper) {
        if (lower < 0 || lower >= n || seen[lower])
            throw std::invalid_argument("lower endpoints must be a permutation");
        seen[lower] = true;
    }
    std::vector<int> length(n, 1);
    std::vector<int> previous(n, -1);
    int best = 0;
    int end = -1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (lowerEndOfUpper[j] < lowerEndOfUpper[i] &&
                length[j] + 1 > length[i]) {
                length[i] = length[j] + 1;
                previous[i] = j;
            }
        }
        if (length[i] > best) {
            best = length[i];
            end = i;
        }
    }
    NoncrossingNetsResult result = {best, {}};
    for (int i = end; i != -1; i = previous[i]) result.upperIndices.push_back(i);
    std::reverse(result.upperIndices.begin(), result.upperIndices.end());
    return result;
}

} // namespace chapter19

#endif
