#include "dynamicProgramming.hpp"

#include <iostream>
#include <vector>

using namespace chapter19;

void printIndices(const std::vector<int>& indices) {
    for (int index : indices) std::cout << ' ' << index;
    std::cout << '\n';
}

int main() {
    const KnapsackResult bag = knapsack01({{10, 60}, {20, 100}, {30, 120}}, 50);
    std::cout << "0/1 knapsack: value=" << bag.maxValue << ", items=";
    printIndices(bag.itemIndices);

    const MatrixChainResult chain = matrixChainOrder({30, 35, 15, 5, 10, 20, 25});
    std::cout << "Matrix chain: cost=" << chain.minCost
              << ", order=" << chain.parenthesization << '\n';

    const long long I = infinity;
    const FloydWarshallResult allPairs = floydWarshall({
        {0, 4, 11, I}, {I, 0, -2, I}, {I, I, 0, 3}, {I, 1, I, 0}
    });
    std::cout << "Floyd-Warshall 0->3: distance=" << allPairs.distance[0][3]
              << ", path=";
    printIndices(restoreFloydPath(allPairs, 0, 3));

    const BellmanFordResult singleSource = bellmanFord(4,
        {{0, 1, 4}, {0, 2, 5}, {1, 2, -2}, {2, 3, 3}}, 0);
    std::cout << "Bellman-Ford 0->3: distance=" << singleSource.distance[3]
              << ", path=";
    printIndices(restoreBellmanFordPath(singleSource, 0, 3));

    const NoncrossingNetsResult nets = maximumNoncrossingNets({3, 0, 2, 1, 4});
    std::cout << "Noncrossing nets: count=" << nets.count << ", upper endpoints=";
    printIndices(nets.upperIndices);
}
