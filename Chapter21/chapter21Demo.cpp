#include "branchAndBound.hpp"

#include <iostream>
#include <vector>

using namespace chapter21;

void print(const std::vector<int>& values) {
    for (int value : values) std::cout << ' ' << value;
    std::cout << '\n';
}

int main() {
    const LoadingResult loading = loadContainers({10, 40, 40}, 50);
    std::cout << "Loading: weight=" << loading.maxWeight << ", containers=";
    print(loading.containerIndices);
    const TwoShipLoadingResult ships = loadTwoShips({10, 40, 40}, 50, 40);
    std::cout << "Two ships: feasible=" << ships.feasible << ", second ship=";
    print(ships.secondIndices);

    const KnapsackResult bag = knapsack01({{10, 60}, {20, 100}, {30, 120}}, 50);
    std::cout << "0/1 knapsack: value=" << bag.maxValue << ", items=";
    print(bag.itemIndices);

    const CliqueResult clique = maximumClique({
        {0, 1, 1, 0}, {1, 0, 1, 0}, {1, 1, 0, 1}, {0, 0, 1, 0}
    });
    std::cout << "Maximum clique: size=" << clique.size << ", vertices=";
    print(clique.vertices);

    const TspResult tour = travelingSalesperson({
        {0, 10, 15, 20}, {10, 0, 35, 25}, {15, 35, 0, 30}, {20, 25, 30, 0}
    });
    std::cout << "TSP: cost=" << tour.minCost << ", tour=";
    print(tour.tour);

    const BoardResult arrangement = arrangeCircuitBoards({
        {1, 0}, {0, 1}, {1, 0}, {0, 1}
    });
    std::cout << "Circuit boards: density=" << arrangement.minDensity
              << ", order=";
    print(arrangement.order);
}
