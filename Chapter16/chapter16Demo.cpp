#include "graph.hpp"
#include "graphAlgorithms.hpp"

#include <iostream>
#include <vector>

void printVertices(const char* name, const std::vector<int>& vertices) {
    std::cout << name << ":";
    for (std::size_t i = 0; i < vertices.size(); ++i) std::cout << ' ' << vertices[i];
    std::cout << '\n';
}

int main() {
    adjacencyArrayGraph g(6, false, false);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 3);
    g.addEdge(3, 4); // 顶点 5 孤立
    printVertices("DFS", depthFirstSearch(g, 0));
    printVertices("BFS", breadthFirstSearch(g, 0));
    printVertices("Path 0 -> 4", findPath(g, 0, 4));
    std::cout << "components: " << connectedComponents(g).size() << '\n';
    std::cout << "forest edges: " << spanningForest(g).size() << '\n';

    adjacencyListGraph weighted(4, false, true);
    weighted.addEdge(0, 1, 4);
    weighted.addEdge(0, 2, 2);
    weighted.addEdge(1, 2, 1);
    weighted.addEdge(1, 3, 5);
    weighted.addEdge(2, 3, 8);
    const minimumSpanningResult mst = minimumSpanningForest(weighted);
    std::cout << "minimum spanning tree weight: " << mst.totalWeight << '\n';
}
