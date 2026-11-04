#include "greedyAlgorithms.hpp"

#include <cassert>
#include <climits>
#include <stdexcept>
#include <vector>

void testContainerLoading() {
    const std::vector<int> chosen = loadContainers({4, 8, 1, 3, 2}, 7);
    assert((chosen == std::vector<int>{2, 4, 3})); // 1+2+3，数量最多
    assert(loadContainers({}, 5).empty());
    assert(loadContainers({5}, 0).empty());
    bool invalid = false;
    try { loadContainers({0}, 3); } catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
}

void testKnapsack() {
    const std::vector<knapsackItem> items{{10, 60}, {20, 100}, {30, 120}};
    const knapsackResult heuristic = greedyKnapsack(items, 50);
    const knapsackResult exact = optimalKnapsack(items, 50);
    assert(heuristic.totalValue == 160);
    assert(exact.totalValue == 220);
    assert((exact.indices == std::vector<int>{1, 2}));
    assert(optimalKnapsack(items, 0).totalValue == 0);
    assert(optimalKnapsack({}, 50).indices.empty());
    bool invalid = false;
    try { optimalKnapsack({{0, 1}}, 5); } catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
}

void testTopologicalSort() {
    const std::vector<directedEdge> edges{{0, 2}, {1, 2}, {2, 3}};
    assert((topologicalSort(5, edges) == std::vector<int>{0, 1, 2, 3, 4}));
    assert(topologicalSort(0, {}).empty());
    bool cycle = false;
    try { topologicalSort(2, {{0, 1}, {1, 0}}); }
    catch (const std::invalid_argument&) { cycle = true; }
    assert(cycle);
}

void testBipartiteCover() {
    // L0->{0,1}, L1->{1,2}, L2->{2,3}；依次选 L0、L2。
    const std::vector<std::vector<int> > neighbors{{0, 1}, {1, 2}, {2, 3}};
    const bipartiteCoverResult result = greedyBipartiteCover(4, neighbors);
    assert((result.leftVertices == std::vector<int>{0, 2}));
    assert(result.allCovered);
    assert(greedyBipartiteCover(0, {}).allCovered);
    assert((greedyBipartiteCover(2, {{0, 0, 1}}).leftVertices == std::vector<int>{0}));
    // 最优为 L1+L2（2 个）；贪婪先选 L0，再选 L1、L2（3 个）。
    const bipartiteCoverResult suboptimal = greedyBipartiteCover(6, {
        {0, 1, 2, 3}, {0, 1, 4}, {2, 3, 5}
    });
    assert(suboptimal.allCovered && suboptimal.leftVertices.size() == 3);
    const bipartiteCoverResult impossible = greedyBipartiteCover(2, {{0}});
    assert(!impossible.allCovered);
    bool invalid = false;
    try { greedyBipartiteCover(2, {{2}}); }
    catch (const std::out_of_range&) { invalid = true; }
    assert(invalid);
}

void testShortestPaths() {
    const std::vector<weightedDirectedEdge> edges{
        {0, 1, 8}, {0, 2, 2}, {2, 1, 1}, {1, 3, 3}, {2, 3, 10}
    };
    const shortestPathResult result = dijkstra(5, edges, 0);
    assert((result.distance == std::vector<long long>{0, 3, 2, 6, LLONG_MAX}));
    assert((restorePath(result, 0, 3) == std::vector<int>{0, 2, 1, 3}));
    assert(restorePath(result, 0, 4).empty());
    assert((restorePath(result, 0, 0) == std::vector<int>{0}));
    const shortestPathResult zero = dijkstra(2, {{0, 1, 0}}, 0);
    assert(zero.distance[1] == 0);
    bool invalid = false;
    try { dijkstra(2, {{0, 1, -1}}, 0); }
    catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
}

void testMinimumSpanningTree() {
    const std::vector<weightedUndirectedEdge> edges{
        {0, 1, 4}, {0, 2, 2}, {1, 2, 1}, {1, 3, 5}, {2, 3, 8}
    };
    const minimumSpanningTreeResult result = kruskal(4, edges);
    assert(result.totalCost == 8);
    assert(result.edges.size() == 3);
    assert(kruskal(1, {}).edges.empty());
    const minimumSpanningTreeResult negative = kruskal(2, {{0, 0, -9}, {0, 1, -3}});
    assert(negative.totalCost == -3 && negative.edges.size() == 1);
    bool disconnected = false;
    try { kruskal(3, {{0, 1, 1}}); }
    catch (const std::invalid_argument&) { disconnected = true; }
    assert(disconnected);
}

int main() {
    testContainerLoading();
    testKnapsack();
    testTopologicalSort();
    testBipartiteCover();
    testShortestPaths();
    testMinimumSpanningTree();
}
