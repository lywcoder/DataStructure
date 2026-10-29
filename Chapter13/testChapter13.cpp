#include "winnerTree.hpp"
#include "loserTree.hpp"
#include "binPacking.hpp"
#include <cassert>
#include <functional>
#include <random>
#include <stdexcept>
#include <vector>

int main() {
    winnerTree<int> winners({7, 2, 9, 4});
    assert(winners.winnerIndex() == 1 && winners.winner() == 2);
    winners.update(1, 10);
    assert(winners.winnerIndex() == 3);
    winners.initialize({8, 3, 3, 6, 1});
    assert(winners.winnerIndex() == 4);
    winners.update(4, 9);
    assert(winners.winnerIndex() == 1); // 相等时下标较小者获胜
    winners.initialize({});
    assert(winners.empty());
    bool threw = false;
    try { winners.winnerIndex(); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);
    threw = false;
    try { winners.update(0, 1); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);

    winnerTree<int, std::greater<int>> maxTree({6, 2, 6, 1, 5});
    assert(maxTree.winnerIndex() == 0);
    assert(maxTree.firstIndexAtLeast(5) == 0);
    assert(maxTree.firstIndexAtLeast(7) == -1);
    maxTree.update(0, 0);
    assert(maxTree.firstIndexAtLeast(5) == 2);

    loserTree<int> losers({7, 2, 9, 4});
    assert(losers.winnerIndex() == 1);
    losers.replaceWinner(10);
    assert(losers.winnerIndex() == 3 && losers.winner() == 4);
    losers.replaceWinner(11);
    assert(losers.winnerIndex() == 0);
    losers.initialize({8, 3, 3, 6, 1});
    assert(losers.winnerIndex() == 4);
    losers.replaceWinner(9);
    assert(losers.winnerIndex() == 1);
    losers.initialize({5});
    losers.replaceWinner(6);
    assert(losers.winnerIndex() == 0 && losers.winner() == 6);
    losers.initialize({});
    threw = false;
    try { losers.replaceWinner(1); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);

    const PackingResult first = firstFitBinPacking({6, 6, 4, 4}, 10);
    assert((first.binOfItem == std::vector<int>{0, 1, 0, 1}));
    assert((first.remaining == std::vector<int>{0, 0}));
    const PackingResult next = nextFitBinPacking({6, 6, 4, 4}, 10);
    assert((next.binOfItem == std::vector<int>{0, 1, 1, 2}));
    assert((next.remaining == std::vector<int>{4, 0, 6}));
    assert(firstFitBinPacking({}, 10).remaining.empty());
    assert(nextFitBinPacking({}, 10).remaining.empty());
    threw = false;
    try { firstFitBinPacking({11}, 10); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { nextFitBinPacking({1}, 0); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { firstFitBinPacking({0}, 10); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    std::mt19937 rng(2026);
    for (int count = 1; count <= 12; ++count) {
        std::vector<int> values(count);
        for (int& value : values) value = static_cast<int>(rng() % 20);
        winnerTree<int> wt(values);
        loserTree<int> lt(values);
        for (int step = 0; step < 80; ++step) {
            int expected = 0;
            for (int i = 1; i < count; ++i)
                if (values[i] < values[expected]) expected = i;
            assert(wt.winnerIndex() == expected);
            assert(lt.winnerIndex() == expected);
            // 输者树每次替换冠军；赢者树更新同一个位置。
            const int replacement = static_cast<int>(rng() % 20);
            lt.replaceWinner(replacement);
            wt.update(expected, replacement);
            values[expected] = replacement;
        }
    }

    for (int trial = 0; trial < 100; ++trial) {
        std::vector<int> items;
        for (int i = 0; i < 30; ++i)
            items.push_back(1 + static_cast<int>(rng() % 10));
        std::vector<int> expectedBins, expectedRemaining;
        for (int size : items) {
            int bin = 0;
            while (bin < static_cast<int>(expectedRemaining.size()) &&
                   expectedRemaining[bin] < size) ++bin;
            if (bin == static_cast<int>(expectedRemaining.size()))
                expectedRemaining.push_back(10);
            expectedRemaining[bin] -= size;
            expectedBins.push_back(bin);
        }
        const PackingResult actual = firstFitBinPacking(items, 10);
        assert(actual.binOfItem == expectedBins);
        assert(actual.remaining == expectedRemaining);
    }
}
