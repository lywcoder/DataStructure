#include "greedyAlgorithms.hpp"

#include <iostream>
#include <vector>

void printIndices(const std::vector<int>& indices) {
    for (int index : indices) std::cout << ' ' << index;
    std::cout << '\n';
}

int main() {
    std::cout << "Container indices:";
    printIndices(loadContainers({4, 8, 1, 3, 2}, 7));

    const std::vector<knapsackItem> items{{10, 60}, {20, 100}, {30, 120}};
    const knapsackResult greedy = greedyKnapsack(items, 50);
    const knapsackResult optimal = optimalKnapsack(items, 50);
    std::cout << "0/1 knapsack greedy value: " << greedy.totalValue << '\n';
    std::cout << "0/1 knapsack optimal value: " << optimal.totalValue << '\n';
    std::cout << "Optimal item indices:";
    printIndices(optimal.indices);

    std::cout << "Topological order:";
    printIndices(topologicalSort(5, {{0, 2}, {1, 2}, {2, 3}}));

    const bipartiteCoverResult cover = greedyBipartiteCover(
        4, {{0, 1}, {1, 2}, {2, 3}});
    std::cout << "Cover left vertices:";
    printIndices(cover.leftVertices);
    std::cout << "All right vertices covered: " << (cover.allCovered ? "yes" : "no") << '\n';

    const shortestPathResult paths = dijkstra(5, {
        {0, 1, 8}, {0, 2, 2}, {2, 1, 1}, {1, 3, 3}, {2, 3, 10}
    }, 0);
    std::cout << "Shortest distance from 0 to 3: " << paths.distance[3] << '\n';
    std::cout << "Shortest path to 3:";
    printIndices(restorePath(paths, 0, 3));

    const minimumSpanningTreeResult tree = kruskal(4, {
        {0, 1, 4}, {0, 2, 2}, {1, 2, 1}, {1, 3, 5}, {2, 3, 8}
    });
    std::cout << "Minimum spanning tree cost: " << tree.totalCost << '\n';
}
