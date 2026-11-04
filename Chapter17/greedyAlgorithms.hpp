#ifndef CHAPTER17_GREEDY_ALGORITHMS_HPP
#define CHAPTER17_GREEDY_ALGORITHMS_HPP

#include <algorithm>
#include <climits>
#include <functional>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

// 目标是装入最多件，而非让装载重量最接近容量。
inline std::vector<int> loadContainers(const std::vector<int>& weights, int capacity) {
    if (capacity < 0) throw std::invalid_argument("negative capacity");
    std::vector<int> order(weights.size());
    for (std::size_t i = 0; i < weights.size(); ++i) {
        if (weights[i] <= 0) throw std::invalid_argument("container weight must be positive");
        order[i] = static_cast<int>(i);
    }
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        if (weights[a] != weights[b]) return weights[a] < weights[b];
        return a < b;
    });
    std::vector<int> chosen;
    int remaining = capacity;
    for (int index : order) {
        if (weights[index] > remaining) break; // 后续只会更重
        chosen.push_back(index);
        remaining -= weights[index];
    }
    return chosen;
}

struct knapsackItem { int weight; int value; };
struct knapsackResult {
    std::vector<int> indices;
    int totalWeight;
    long long totalValue;
};

inline void validateKnapsack(const std::vector<knapsackItem>& items, int capacity) {
    if (capacity < 0) throw std::invalid_argument("negative capacity");
    for (const knapsackItem& item : items)
        if (item.weight <= 0 || item.value < 0)
            throw std::invalid_argument("weight must be positive and value nonnegative");
}

// 单位重量价值优先，只是 0/1 背包的启发式，不保证最优。
inline knapsackResult greedyKnapsack(const std::vector<knapsackItem>& items, int capacity) {
    validateKnapsack(items, capacity);
    std::vector<int> order(items.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        const long long left = static_cast<long long>(items[a].value) * items[b].weight;
        const long long right = static_cast<long long>(items[b].value) * items[a].weight;
        return left != right ? left > right : a < b;
    });
    knapsackResult result = {{}, 0, 0};
    for (int index : order) {
        if (items[index].weight > capacity - result.totalWeight) continue;
        result.indices.push_back(index);
        result.totalWeight += items[index].weight;
        result.totalValue += items[index].value;
    }
    return result;
}

// dp[i][c]：前 i 件物品、容量 c 的最大价值；最后逆推选中的物品。
inline knapsackResult optimalKnapsack(const std::vector<knapsackItem>& items, int capacity) {
    validateKnapsack(items, capacity);
    const std::size_t n = items.size();
    std::vector<std::vector<long long> > dp(n + 1, std::vector<long long>(capacity + 1, 0));
    for (std::size_t i = 1; i <= n; ++i) {
        for (int c = 0; c <= capacity; ++c) {
            dp[i][c] = dp[i - 1][c]; // 不选第 i 件
            if (items[i - 1].weight <= c) {
                const long long take = dp[i - 1][c - items[i - 1].weight] + items[i - 1].value;
                if (take > dp[i][c]) dp[i][c] = take;
            }
        }
    }
    knapsackResult result = {{}, 0, dp[n][capacity]};
    int c = capacity;
    for (std::size_t i = n; i > 0; --i) {
        if (dp[i][c] == dp[i - 1][c]) continue; // 平局时固定不选
        result.indices.push_back(static_cast<int>(i - 1));
        result.totalWeight += items[i - 1].weight;
        c -= items[i - 1].weight;
    }
    std::reverse(result.indices.begin(), result.indices.end());
    return result;
}

struct directedEdge { int from; int to; };

inline void checkVertex(int vertex, int vertexCount) {
    if (vertex < 0 || vertex >= vertexCount) throw std::out_of_range("invalid vertex");
}

// 每次取入度为 0 的点；若仍有点未取出，就存在有向环。
inline std::vector<int> topologicalSort(int vertexCount, const std::vector<directedEdge>& edges) {
    if (vertexCount < 0) throw std::invalid_argument("negative vertex count");
    std::vector<std::vector<int> > next(vertexCount);
    std::vector<int> indegree(vertexCount, 0);
    for (const directedEdge& edge : edges) {
        checkVertex(edge.from, vertexCount);
        checkVertex(edge.to, vertexCount);
        next[edge.from].push_back(edge.to);
        ++indegree[edge.to];
    }
    std::priority_queue<int, std::vector<int>, std::greater<int> > ready;
    for (int v = 0; v < vertexCount; ++v) if (indegree[v] == 0) ready.push(v);
    std::vector<int> order;
    while (!ready.empty()) {
        const int v = ready.top();
        ready.pop();
        order.push_back(v);
        for (int to : next[v]) if (--indegree[to] == 0) ready.push(to);
    }
    if (static_cast<int>(order.size()) != vertexCount)
        throw std::invalid_argument("graph contains a directed cycle");
    return order;
}

struct bipartiteCoverResult {
    std::vector<int> leftVertices;
    bool allCovered;
};

// 左侧顶点代表集合，右侧顶点代表待覆盖元素；每轮选新增覆盖最多的集合。
inline bipartiteCoverResult greedyBipartiteCover(
    int rightCount, const std::vector<std::vector<int> >& neighbors) {
    if (rightCount < 0) throw std::invalid_argument("negative right vertex count");
    std::vector<std::vector<int> > uniqueNeighbors = neighbors;
    for (std::vector<int>& row : uniqueNeighbors) {
        for (int right : row) checkVertex(right, rightCount);
        std::sort(row.begin(), row.end());
        row.erase(std::unique(row.begin(), row.end()), row.end());
    }
    std::vector<bool> covered(rightCount, false);
    std::vector<bool> selected(neighbors.size(), false);
    bipartiteCoverResult result = {{}, true};
    int uncovered = rightCount;
    while (uncovered > 0) {
        int best = -1;
        int bestGain = 0;
        for (std::size_t left = 0; left < uniqueNeighbors.size(); ++left) {
            if (selected[left]) continue;
            int gain = 0;
            for (int right : uniqueNeighbors[left]) if (!covered[right]) ++gain;
            if (gain > bestGain) { bestGain = gain; best = static_cast<int>(left); }
        }
        if (best == -1) { result.allCovered = false; break; }
        selected[best] = true;
        result.leftVertices.push_back(best);
        for (int right : uniqueNeighbors[best]) {
            if (!covered[right]) { covered[right] = true; --uncovered; }
        }
    }
    return result;
}

struct weightedDirectedEdge { int from; int to; int weight; };
struct shortestPathResult {
    std::vector<long long> distance; // LLONG_MAX 表示不可达
    std::vector<int> parent;          // -1 表示没有前驱
};

inline shortestPathResult dijkstra(
    int vertexCount, const std::vector<weightedDirectedEdge>& edges, int source) {
    if (vertexCount < 0) throw std::invalid_argument("negative vertex count");
    checkVertex(source, vertexCount);
    std::vector<std::vector<std::pair<int, int> > > next(vertexCount);
    for (const weightedDirectedEdge& edge : edges) {
        checkVertex(edge.from, vertexCount);
        checkVertex(edge.to, vertexCount);
        if (edge.weight < 0) throw std::invalid_argument("Dijkstra requires nonnegative weights");
        next[edge.from].push_back(std::make_pair(edge.to, edge.weight));
    }
    shortestPathResult result = {std::vector<long long>(vertexCount, LLONG_MAX),
                                 std::vector<int>(vertexCount, -1)};
    typedef std::pair<long long, int> queueEntry; // (当前距离, 顶点)
    std::priority_queue<queueEntry, std::vector<queueEntry>, std::greater<queueEntry> > ready;
    result.distance[source] = 0;
    ready.push(std::make_pair(0, source));
    while (!ready.empty()) {
        const queueEntry current = ready.top();
        ready.pop();
        const int from = current.second;
        if (current.first != result.distance[from]) continue; // 过期的队列项
        for (const std::pair<int, int>& edge : next[from]) {
            const int to = edge.first;
            const long long candidate = current.first + edge.second;
            if (candidate >= result.distance[to]) continue;
            result.distance[to] = candidate;
            result.parent[to] = from;
            ready.push(std::make_pair(candidate, to));
        }
    }
    return result;
}

inline std::vector<int> restorePath(const shortestPathResult& result, int source, int target) {
    const int count = static_cast<int>(result.distance.size());
    checkVertex(source, count);
    checkVertex(target, count);
    if (result.distance[target] == LLONG_MAX) return {};
    std::vector<int> path;
    for (int at = target; at != -1; at = result.parent[at]) {
        path.push_back(at);
        if (at == source) break;
    }
    if (path.back() != source) return {};
    std::reverse(path.begin(), path.end());
    return path;
}

struct weightedUndirectedEdge { int from; int to; int cost; };
struct minimumSpanningTreeResult {
    std::vector<weightedUndirectedEdge> edges;
    long long totalCost;
};

inline int findRoot(std::vector<int>& parent, int vertex) {
    if (parent[vertex] != vertex) parent[vertex] = findRoot(parent, parent[vertex]);
    return parent[vertex];
}

// Kruskal：按边权升序检查，只有连接两个不同连通分量才选择该边。
inline minimumSpanningTreeResult kruskal(
    int vertexCount, std::vector<weightedUndirectedEdge> edges) {
    if (vertexCount < 0) throw std::invalid_argument("negative vertex count");
    for (const weightedUndirectedEdge& edge : edges) {
        checkVertex(edge.from, vertexCount);
        checkVertex(edge.to, vertexCount);
    }
    std::sort(edges.begin(), edges.end(), [](const weightedUndirectedEdge& a,
                                             const weightedUndirectedEdge& b) {
        if (a.cost != b.cost) return a.cost < b.cost;
        if (a.from != b.from) return a.from < b.from;
        return a.to < b.to;
    });
    std::vector<int> parent(vertexCount), rank(vertexCount, 0);
    std::iota(parent.begin(), parent.end(), 0);
    minimumSpanningTreeResult result = {{}, 0};
    for (const weightedUndirectedEdge& edge : edges) {
        int a = findRoot(parent, edge.from);
        int b = findRoot(parent, edge.to);
        if (a == b) continue; // 加这条边会形成环
        if (rank[a] < rank[b]) std::swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b]) ++rank[a];
        result.edges.push_back(edge);
        result.totalCost += edge.cost;
        if (result.edges.size() == static_cast<std::size_t>(vertexCount - 1)) break;
    }
    if (vertexCount > 0 && result.edges.size() != static_cast<std::size_t>(vertexCount - 1))
        throw std::invalid_argument("graph is disconnected");
    return result;
}

#endif
