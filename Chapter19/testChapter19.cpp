#include "dynamicProgramming.hpp"

#include <cassert>
#include <stdexcept>
#include <vector>

using namespace chapter19;

template <typename Exception, typename Action>
void expectThrows(Action action) {
    bool thrown = false;
    try { action(); }
    catch (const Exception&) { thrown = true; }
    assert(thrown);
}

void testKnapsack() {
    const KnapsackResult result = knapsack01({{10, 60}, {20, 100}, {30, 120}}, 50);
    assert(result.maxValue == 220);
    assert(result.totalWeight == 50);
    assert((result.itemIndices == std::vector<int>{1, 2}));
    assert(knapsack01({}, 10).maxValue == 0);
    assert(knapsack01({{1, 5}}, 0).itemIndices.empty());
    expectThrows<std::invalid_argument>([] { knapsack01({{0, 1}}, 2); });
    expectThrows<std::invalid_argument>([] { knapsack01({{1, -1}}, 2); });
    expectThrows<std::invalid_argument>([] { knapsack01({}, -1); });
}

void testMatrixChain() {
    const MatrixChainResult result = matrixChainOrder({30, 35, 15, 5, 10, 20, 25});
    assert(result.minCost == 15125);
    assert(matrixChainOrder({2, 3}).parenthesization == "A1");
    assert(matrixChainOrder({2, 3, 4}).minCost == 24);
    expectThrows<std::invalid_argument>([] { matrixChainOrder({2}); });
    expectThrows<std::invalid_argument>([] { matrixChainOrder({2, 0}); });
}

void testAllPairsShortestPaths() {
    const long long I = infinity;
    const FloydWarshallResult result = floydWarshall({
        {0, 4, 11, I}, {I, 0, -2, I}, {I, I, 0, 3}, {I, 1, I, 0}
    });
    assert(result.distance[0][3] == 5);
    assert((restoreFloydPath(result, 0, 3) == std::vector<int>{0, 1, 2, 3}));
    assert(restoreFloydPath(result, 3, 0).empty());
    assert((restoreFloydPath(result, 2, 2) == std::vector<int>{2}));
    expectThrows<std::invalid_argument>([I] { floydWarshall({{0, I}, {I}}); });
    expectThrows<std::invalid_argument>([I] { floydWarshall({{0, -2}, {-1, 0}}); });
    expectThrows<std::out_of_range>([&result] { restoreFloydPath(result, -1, 0); });
}

void testNegativeEdgeSingleSource() {
    const BellmanFordResult result = bellmanFord(5,
        {{0, 1, 4}, {0, 2, 5}, {1, 2, -2}, {2, 3, 3}, {4, 4, -1}}, 0);
    assert(result.distance[3] == 5);
    assert(result.distance[4] == infinity);
    assert((restoreBellmanFordPath(result, 0, 3) == std::vector<int>{0, 1, 2, 3}));
    assert(restoreBellmanFordPath(result, 0, 4).empty());
    expectThrows<std::invalid_argument>([] {
        bellmanFord(2, {{0, 1, -2}, {1, 0, -1}}, 0);
    });
    expectThrows<std::out_of_range>([] { bellmanFord(2, {{0, 2, 1}}, 0); });
    expectThrows<std::out_of_range>([] { bellmanFord(2, {}, 2); });
}

void testNoncrossingNets() {
    const NoncrossingNetsResult result = maximumNoncrossingNets({3, 0, 2, 1, 4});
    assert(result.count == 3);
    assert((result.upperIndices == std::vector<int>{1, 2, 4}));
    assert(maximumNoncrossingNets({}).count == 0);
    assert(maximumNoncrossingNets({2, 1, 0}).count == 1);
    expectThrows<std::invalid_argument>([] { maximumNoncrossingNets({0, 0}); });
    expectThrows<std::invalid_argument>([] { maximumNoncrossingNets({1}); });
}

int main() {
    testKnapsack();
    testMatrixChain();
    testAllPairsShortestPaths();
    testNegativeEdgeSingleSource();
    testNoncrossingNets();
}
