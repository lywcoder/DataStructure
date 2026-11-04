#ifndef CHAPTER16_GRAPH_ALGORITHMS_HPP
#define CHAPTER16_GRAPH_ALGORITHMS_HPP

#include "graph.hpp"

#include <algorithm>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

inline void depthFirstVisit(const graph& g, int vertex, std::vector<bool>& visited,
                            std::vector<int>& order) {
    visited[vertex] = true;
    order.push_back(vertex);
    const std::vector<graphEdge> adjacent = g.neighbors(vertex);
    for (std::size_t i = 0; i < adjacent.size(); ++i)
        if (!visited[adjacent[i].to]) depthFirstVisit(g, adjacent[i].to, visited, order);
}

inline std::vector<int> depthFirstSearch(const graph& g, int start) {
    g.neighbors(start); // 统一验证起点
    std::vector<bool> visited(g.vertexCount(), false);
    std::vector<int> order;
    depthFirstVisit(g, start, visited, order);
    return order;
}

inline std::vector<int> breadthFirstSearch(const graph& g, int start) {
    g.neighbors(start);
    std::vector<bool> visited(g.vertexCount(), false);
    std::vector<int> order;
    std::queue<int> pending;
    visited[start] = true;
    pending.push(start);
    while (!pending.empty()) {
        const int vertex = pending.front();
        pending.pop();
        order.push_back(vertex);
        const std::vector<graphEdge> adjacent = g.neighbors(vertex);
        for (std::size_t i = 0; i < adjacent.size(); ++i) {
            const int next = adjacent[i].to;
            if (!visited[next]) {
                visited[next] = true; // 入队时标记，防止同一顶点重复入队
                pending.push(next);
            }
        }
    }
    return order;
}

// BFS 的 parent 链给出边数最少的路径；权值不参与计算。
inline std::vector<int> findPath(const graph& g, int source, int target) {
    g.neighbors(source);
    g.neighbors(target);
    std::vector<int> parent(g.vertexCount(), -1);
    std::queue<int> pending;
    parent[source] = source;
    pending.push(source);
    while (!pending.empty() && parent[target] == -1) {
        const int vertex = pending.front();
        pending.pop();
        const std::vector<graphEdge> adjacent = g.neighbors(vertex);
        for (std::size_t i = 0; i < adjacent.size(); ++i) {
            const int next = adjacent[i].to;
            if (parent[next] == -1) {
                parent[next] = vertex;
                pending.push(next);
            }
        }
    }
    std::vector<int> path;
    if (parent[target] == -1) return path;
    for (int at = target; at != source; at = parent[at]) path.push_back(at);
    path.push_back(source);
    std::reverse(path.begin(), path.end());
    return path;
}

inline std::vector<std::vector<int> > connectedComponents(const graph& g) {
    if (g.directed()) throw std::invalid_argument("connected components require an undirected graph");
    std::vector<bool> visited(g.vertexCount(), false);
    std::vector<std::vector<int> > components;
    for (int start = 0; start < g.vertexCount(); ++start) {
        if (visited[start]) continue;
        std::vector<int> component;
        std::queue<int> pending;
        visited[start] = true;
        pending.push(start);
        while (!pending.empty()) {
            const int vertex = pending.front();
            pending.pop();
            component.push_back(vertex);
            const std::vector<graphEdge> adjacent = g.neighbors(vertex);
            for (std::size_t i = 0; i < adjacent.size(); ++i) {
                const int next = adjacent[i].to;
                if (!visited[next]) { visited[next] = true; pending.push(next); }
            }
        }
        components.push_back(component);
    }
    return components;
}

inline bool isConnected(const graph& g) {
    return connectedComponents(g).size() <= 1; // 约定空图连通
}

// 每个首次发现新顶点的边都是树边；非连通图得到生成森林。
inline std::vector<std::pair<int, int> > spanningForest(const graph& g) {
    if (g.directed()) throw std::invalid_argument("spanning forest requires an undirected graph");
    std::vector<bool> visited(g.vertexCount(), false);
    std::vector<std::pair<int, int> > edges;
    std::queue<int> pending;
    for (int start = 0; start < g.vertexCount(); ++start) {
        if (visited[start]) continue;
        visited[start] = true;
        pending.push(start);
        while (!pending.empty()) {
            const int vertex = pending.front();
            pending.pop();
            const std::vector<graphEdge> adjacent = g.neighbors(vertex);
            for (std::size_t i = 0; i < adjacent.size(); ++i) {
                const int next = adjacent[i].to;
                if (!visited[next]) {
                    visited[next] = true;
                    edges.push_back(std::make_pair(vertex, next));
                    pending.push(next);
                }
            }
        }
    }
    return edges;
}

struct weightedTreeEdge { int from; int to; int weight; };
struct minimumSpanningResult {
    std::vector<weightedTreeEdge> edges;
    long long totalWeight;
    int componentCount;
};

// Kruskal：按权值取边，只有连接不同分量时才接受。
inline minimumSpanningResult minimumSpanningForest(const graph& g) {
    if (g.directed() || !g.weighted())
        throw std::invalid_argument("Kruskal requires a weighted undirected graph");
    std::vector<weightedTreeEdge> candidates;
    for (int from = 0; from < g.vertexCount(); ++from) {
        const std::vector<graphEdge> adjacent = g.neighbors(from);
        for (std::size_t i = 0; i < adjacent.size(); ++i)
            if (from < adjacent[i].to)
                candidates.push_back(weightedTreeEdge{from, adjacent[i].to, adjacent[i].weight});
    }
    std::sort(candidates.begin(), candidates.end(),
              [](const weightedTreeEdge& a, const weightedTreeEdge& b) {
                  if (a.weight != b.weight) return a.weight < b.weight;
                  if (a.from != b.from) return a.from < b.from;
                  return a.to < b.to;
              });
    std::vector<int> parent(g.vertexCount());
    std::vector<int> rank(g.vertexCount(), 0);
    std::iota(parent.begin(), parent.end(), 0);
    minimumSpanningResult result = {{}, 0, g.vertexCount()};
    for (std::size_t i = 0; i < candidates.size(); ++i) {
        int a = candidates[i].from;
        int b = candidates[i].to;
        while (parent[a] != a) { parent[a] = parent[parent[a]]; a = parent[a]; }
        while (parent[b] != b) { parent[b] = parent[parent[b]]; b = parent[b]; }
        if (a == b) continue;
        if (rank[a] < rank[b]) std::swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b]) ++rank[a];
        result.edges.push_back(candidates[i]);
        result.totalWeight += candidates[i].weight;
        --result.componentCount;
    }
    return result;
}

#endif
