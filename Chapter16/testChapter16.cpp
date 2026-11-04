#include "graph.hpp"
#include "graphAlgorithms.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>

template<class G>
void testRepresentation() {
    G g(5, false, true);
    assert(g.vertexCount() == 5 && g.edgeCount() == 0);
    g.addEdge(0, 1, 7);
    g.addEdge(0, 2, 3);
    g.addEdge(1, 3, 2);
    g.addEdge(0, 1, 9); // 重复边更新权值，不增加边数
    assert(g.edgeCount() == 3);
    assert(g.hasEdge(1, 0) && g.weight(1, 0) == 9);
    assert(g.weight(0, 2) == 3);
    assert(!g.hasEdge(4, 0));
    bool invalid = false;
    try { g.addEdge(0, 5); } catch (const std::out_of_range&) { invalid = true; }
    assert(invalid);
    G plain(2, true, false);
    plain.addEdge(0, 1);
    assert(plain.hasEdge(0, 1) && !plain.hasEdge(1, 0));
    invalid = false;
    try { plain.addEdge(0, 1, 4); } catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
    G loop(1, false, false);
    loop.addEdge(0, 0);
    assert(loop.edgeCount() == 1 && loop.neighbors(0).size() == 1);
}

template<class G>
void testAlgorithms() {
    G g(6, false, false);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    assert((depthFirstSearch(g, 0) == std::vector<int>{0, 1, 3, 2, 4}));
    assert((breadthFirstSearch(g, 0) == std::vector<int>{0, 1, 2, 3, 4}));
    assert((findPath(g, 0, 4) == std::vector<int>{0, 1, 3, 4}));
    assert((findPath(g, 0, 0) == std::vector<int>{0}));
    assert(findPath(g, 0, 5).empty());
    auto components = connectedComponents(g);
    assert((components == std::vector<std::vector<int>>{{0, 1, 2, 3, 4}, {5}}));
    assert(!isConnected(g));
    auto forest = spanningForest(g);
    assert(forest.size() == 4);
    assert(forest[0].first == 0 && forest[0].second == 1);
    G empty(0, false, false);
    assert(connectedComponents(empty).empty() && isConnected(empty));
    assert(spanningForest(empty).empty());
    bool invalid = false;
    try { breadthFirstSearch(g, 6); } catch (const std::out_of_range&) { invalid = true; }
    assert(invalid);
}

void testMinimumSpanningForest() {
    adjacencyListGraph g(5, false, true);
    g.addEdge(0, 1, 4);
    g.addEdge(0, 2, 2);
    g.addEdge(1, 2, 1);
    g.addEdge(1, 3, 5);
    g.addEdge(2, 3, 8);
    auto result = minimumSpanningForest(g);
    assert(result.totalWeight == 8);
    assert(result.edges.size() == 3);
    assert(result.componentCount == 2);
    adjacencyMatrixGraph directed(2, true, true);
    bool invalid = false;
    try { minimumSpanningForest(directed); } catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
    try { connectedComponents(directed); assert(false); } catch (const std::invalid_argument&) {}
}

void testDirectedAndZeroWeight() {
    adjacencyMatrixGraph matrix(4, true, true);
    matrix.addEdge(0, 1, 0);
    matrix.addEdge(1, 2, -3);
    matrix.addEdge(2, 0, 5);
    assert(matrix.hasEdge(0, 1) && matrix.weight(0, 1) == 0);
    assert(!matrix.hasEdge(1, 0));
    assert((depthFirstSearch(matrix, 0) == std::vector<int>{0, 1, 2}));
    assert(findPath(matrix, 2, 1) == (std::vector<int>{2, 0, 1}));
    assert(findPath(matrix, 1, 3).empty());

    adjacencyArrayGraph weighted(3, false, true);
    weighted.addEdge(0, 1, -2);
    weighted.addEdge(1, 2, 0);
    weighted.addEdge(0, 2, 9);
    const minimumSpanningResult tree = minimumSpanningForest(weighted);
    assert(tree.componentCount == 1 && tree.edges.size() == 2);
    assert(tree.totalWeight == -2);
    bool invalid = false;
    try { weighted.weight(0, 0); } catch (const std::out_of_range&) { invalid = true; }
    assert(invalid);
}

int main() {
    testRepresentation<adjacencyMatrixGraph>();
    testRepresentation<adjacencyListGraph>();
    testRepresentation<adjacencyArrayGraph>();
    testAlgorithms<adjacencyMatrixGraph>();
    testAlgorithms<adjacencyListGraph>();
    testAlgorithms<adjacencyArrayGraph>();
    testMinimumSpanningForest();
    testDirectedAndZeroWeight();
}
