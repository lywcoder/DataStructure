#include "backtracking.hpp"

#include <cassert>
#include <stdexcept>
#include <vector>

using namespace chapter20;

template <typename Exception, typename Action>
void expectThrows(Action action) {
    bool thrown = false;
    try { action(); }
    catch (const Exception&) { thrown = true; }
    assert(thrown);
}

void testKnapsack() {
    const KnapsackResult answer = knapsack01({{10, 60}, {20, 100}, {30, 120}}, 50);
    assert(answer.maxValue == 220);
    assert(answer.totalWeight == 50);
    assert((answer.itemIndices == std::vector<int>{1, 2}));
    assert(knapsack01({}, 8).maxValue == 0);
    assert(knapsack01({{2, 3}}, 1).itemIndices.empty());
    expectThrows<std::invalid_argument>([] { knapsack01({{0, 1}}, 3); });
    expectThrows<std::invalid_argument>([] { knapsack01({}, -1); });
}

void testMaximumClique() {
    const CliqueResult answer = maximumClique({
        {0, 1, 1, 0}, {1, 0, 1, 0}, {1, 1, 0, 1}, {0, 0, 1, 0}
    });
    assert(answer.size == 3);
    assert((answer.vertices == std::vector<int>{0, 1, 2}));
    assert(maximumClique({}).size == 0);
    assert(maximumClique({{0, 0}, {0, 0}}).size == 1);
    expectThrows<std::invalid_argument>([] { maximumClique({{0, 1}, {0, 0}}); });
}

void testTravelingSalesperson() {
    const TspResult answer = travelingSalesperson({
        {0, 10, 15, 20}, {10, 0, 35, 25}, {15, 35, 0, 30}, {20, 25, 30, 0}
    });
    assert(answer.minCost == 80);
    assert(answer.tour.front() == 0 && answer.tour.back() == 0);
    assert(answer.tour.size() == 5);
    assert(travelingSalesperson({{0, 1, -1}, {-1, 0, 1}, {-1, -1, 0}}).tour.empty());
    assert(travelingSalesperson({{0}}).minCost == 0);
    expectThrows<std::invalid_argument>([] { travelingSalesperson({{0, -2}, {1, 0}}); });
}

void testCircuitBoardArrangement() {
    const BoardResult answer = arrangeCircuitBoards({
        {1, 0}, {0, 1}, {1, 0}, {0, 1}
    });
    assert(answer.minDensity == 1);
    assert(answer.order.size() == 4);
    assert(arrangeCircuitBoards({}).minDensity == 0);
    assert(arrangeCircuitBoards({{}, {}}).minDensity == 0);
    expectThrows<std::invalid_argument>([] { arrangeCircuitBoards({{1}, {1, 0}}); });
    expectThrows<std::invalid_argument>([] { arrangeCircuitBoards({{2}}); });
}

int main() {
    testKnapsack();
    testMaximumClique();
    testTravelingSalesperson();
    testCircuitBoardArrangement();
}
