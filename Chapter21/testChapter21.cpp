#include "branchAndBound.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>

using namespace chapter21;

template <typename Exception, typename Action>
void expectThrows(Action action) {
    bool thrown = false;
    try { action(); }
    catch (const Exception&) { thrown = true; }
    assert(thrown);
}

void testLoading() {
    const LoadingResult result = loadContainers({10, 40, 40}, 50);
    assert(result.maxWeight == 50);
    assert((result.containerIndices == std::vector<int>{0, 1} ||
            result.containerIndices == std::vector<int>{0, 2}));
    assert(loadContainers({}, 5).maxWeight == 0);
    assert(loadContainers({3, 4}, 0).containerIndices.empty());
    assert(loadContainers({6, 4, 4}, 8).maxWeight == 8);
    expectThrows<std::invalid_argument>([] { loadContainers({0}, 3); });
    expectThrows<std::invalid_argument>([] { loadContainers({1}, -1); });
}

void testTwoShipLoading() {
    const TwoShipLoadingResult result = loadTwoShips({10, 40, 40}, 50, 40);
    assert(result.feasible);
    assert(result.firstWeight == 50 && result.secondWeight == 40);
    assert((result.firstIndices == std::vector<int>{0, 1} ||
            result.firstIndices == std::vector<int>{0, 2}));
    assert(result.secondIndices.size() == 1);
    assert(!loadTwoShips({10, 40, 40}, 45, 40).feasible);
    assert(loadTwoShips({}, 0, 0).feasible);
    expectThrows<std::invalid_argument>([] { loadTwoShips({1}, 2, -1); });
}

void testKnapsack() {
    const KnapsackResult result = knapsack01({{10, 60}, {20, 100}, {30, 120}}, 50);
    assert(result.maxValue == 220);
    assert(result.totalWeight == 50);
    assert((result.itemIndices == std::vector<int>{1, 2}));
    assert(knapsack01({}, 5).maxValue == 0);
    assert(knapsack01({{2, 3}}, 1).itemIndices.empty());
    assert(knapsack01({{4, 5}, {3, 6}, {2, 4}}, 5).maxValue == 10);
    expectThrows<std::invalid_argument>([] { knapsack01({{0, 2}}, 3); });
    expectThrows<std::invalid_argument>([] { knapsack01({{1, -1}}, 3); });
}

void testClique() {
    const CliqueResult result = maximumClique({
        {0, 1, 1, 0}, {1, 0, 1, 0}, {1, 1, 0, 1}, {0, 0, 1, 0}
    });
    assert(result.size == 3);
    assert((result.vertices == std::vector<int>{0, 1, 2}));
    assert(maximumClique({}).vertices.empty());
    assert(maximumClique({{0, 0}, {0, 0}}).size == 1);
    assert(maximumClique({{0, 1, 1}, {1, 0, 0}, {1, 0, 0}}).size == 2);
    expectThrows<std::invalid_argument>([] { maximumClique({{0, 1}, {0, 0}}); });
}

void testTravelingSalesperson() {
    const TspResult result = travelingSalesperson({
        {0, 10, 15, 20}, {10, 0, 35, 25}, {15, 35, 0, 30}, {20, 25, 30, 0}
    });
    assert(result.minCost == 80);
    assert(result.tour.size() == 5);
    assert(result.tour.front() == 0 && result.tour.back() == 0);
    std::vector<int> visited(result.tour.begin(), result.tour.end() - 1);
    std::sort(visited.begin(), visited.end());
    assert((visited == std::vector<int>{0, 1, 2, 3}));
    assert(travelingSalesperson({{0, 1, -1}, {-1, 0, 1}, {-1, -1, 0}}).minCost == -1);
    assert(travelingSalesperson({
        {0, 1, -1, -1}, {1, 0, -1, -1},
        {-1, -1, 0, 1}, {-1, -1, 1, 0}
    }).minCost == -1);
    assert(travelingSalesperson({{0, 0}, {2, 0}}).minCost == 2);
    assert((travelingSalesperson({{0}}).tour == std::vector<int>{0, 0}));
    expectThrows<std::invalid_argument>([] { travelingSalesperson({{0, -2}, {1, 0}}); });
}

void testBoardArrangement() {
    const BoardResult result = arrangeCircuitBoards({
        {1, 0}, {0, 1}, {1, 0}, {0, 1}
    });
    assert(result.minDensity == 1);
    assert(result.order.size() == 4);
    assert(arrangeCircuitBoards({{1}, {1}, {1}}).minDensity == 1);
    assert(arrangeCircuitBoards({{1, 0}, {1, 1}, {0, 1}}).minDensity == 1);
    assert(arrangeCircuitBoards({}).minDensity == 0);
    assert(arrangeCircuitBoards({{}, {}}).minDensity == 0);
    expectThrows<std::invalid_argument>([] { arrangeCircuitBoards({{1}, {1, 0}}); });
    expectThrows<std::invalid_argument>([] { arrangeCircuitBoards({{2}}); });
}

int main() {
    testLoading();
    testTwoShipLoading();
    testKnapsack();
    testClique();
    testTravelingSalesperson();
    testBoardArrangement();
}
