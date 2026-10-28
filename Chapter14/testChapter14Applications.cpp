#include "binarySearchTree.hpp"
#include "histogram.hpp"
#include "bestFitBinPacking.hpp"
#include "crossingDistribution.hpp"
#include <algorithm>
#include <cassert>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

int main() {
    binarySearchTree<int, int> ordered;
    ordered.insert({20, 1});
    ordered.insert({10, 2});
    ordered.insert({30, 3});
    assert(ordered.lowerBound(19)->first == 20);
    assert(ordered.lowerBound(20)->first == 20);
    assert(ordered.lowerBound(31) == nullptr);

    const auto counts = histogram(std::vector<int>{2, 4, 2, 2, 3, 4, 2, 6, 4, 2});
    assert((counts == std::vector<std::pair<int, std::size_t>>{
        {2, 5}, {3, 1}, {4, 3}, {6, 1}}));
    assert(histogram(std::vector<int>{}).empty());

    const BinPackingResult packed = bestFitBinPacking({4, 8, 1, 4, 2, 1}, 10);
    assert((packed.binOfItem == std::vector<int>{0, 1, 1, 0, 0, 1}));
    assert((packed.remaining == std::vector<int>{0, 0}));
    const BinPackingResult tied = bestFitBinPacking({6, 6, 4}, 10);
    assert((tied.binOfItem == std::vector<int>{0, 1, 0}));
    const BinPackingResult heuristic = bestFitBinPacking({6, 5, 3, 2, 4}, 10);
    assert(heuristic.remaining.size() == 3); // 全局最优可以只用两个箱子
    bool badSize = false;
    try { bestFitBinPacking({11}, 10); }
    catch (const std::invalid_argument&) { badSize = true; }
    assert(badSize);

    std::mt19937 rng(173);
    for (int trial = 0; trial < 100; ++trial) {
        std::vector<int> items;
        for (int i = 0; i < 20; ++i) items.push_back(1 + static_cast<int>(rng() % 10));
        std::vector<int> expectedBins, expectedRemaining;
        for (int item : items) {
            int choice = -1;
            for (int bin = 0; bin < static_cast<int>(expectedRemaining.size()); ++bin)
                if (expectedRemaining[bin] >= item &&
                    (choice < 0 || expectedRemaining[bin] < expectedRemaining[choice]))
                    choice = bin;
            if (choice < 0) {
                choice = static_cast<int>(expectedRemaining.size());
                expectedRemaining.push_back(10);
            }
            expectedRemaining[choice] -= item;
            expectedBins.push_back(choice);
        }
        const BinPackingResult actual = bestFitBinPacking(items, 10);
        assert(actual.binOfItem == expectedBins && actual.remaining == expectedRemaining);
    }

    const CrossingDistribution crossing = crossingDistribution({8, 7, 4, 2, 5, 1, 9, 3, 10, 6});
    assert((crossing.perWire == std::vector<int>{7, 6, 3, 1, 2, 0, 2, 0, 1, 0}));
    assert(crossing.total == 22);
    assert(crossingDistribution({1, 2, 3}).total == 0);
    assert(crossingDistribution({3, 2, 1}).total == 3);
    bool badPermutation = false;
    try { crossingDistribution({1, 1, 2}); }
    catch (const std::invalid_argument&) { badPermutation = true; }
    assert(badPermutation);

    for (int n = 0; n <= 6; ++n) {
        std::vector<int> permutation(n);
        std::iota(permutation.begin(), permutation.end(), 1);
        do {
            std::vector<int> expected(n, 0);
            int total = 0;
            for (int i = 0; i < n; ++i)
                for (int j = i + 1; j < n; ++j)
                    if (permutation[i] > permutation[j]) { ++expected[i]; ++total; }
            const CrossingDistribution actual = crossingDistribution(permutation);
            assert(actual.perWire == expected && actual.total == total);
        } while (std::next_permutation(permutation.begin(), permutation.end()));
    }
    std::vector<int> sortedWires(20000);
    std::iota(sortedWires.begin(), sortedWires.end(), 1);
    const CrossingDistribution noCrossings = crossingDistribution(sortedWires);
    assert(noCrossings.total == 0 && noCrossings.perWire.size() == sortedWires.size());
}
