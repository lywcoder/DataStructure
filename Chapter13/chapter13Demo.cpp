#include "winnerTree.hpp"
#include "loserTree.hpp"
#include "binPacking.hpp"
#include <iostream>
#include <vector>

int main() {
    const std::vector<int> scores{7, 2, 9, 4};
    winnerTree<int> winners(scores);
    loserTree<int> losers(scores);
    std::cout << "Initial champion: player " << winners.winnerIndex()
              << " with score " << winners.winner() << '\n';

    winners.update(1, 10);
    losers.replaceWinner(10);
    std::cout << "After replacing player 1's score with 10: winner tree = "
              << winners.winnerIndex() << ", loser tree = "
              << losers.winnerIndex() << '\n';

    const std::vector<int> items{6, 6, 4, 4};
    const PackingResult first = firstFitBinPacking(items, 10);
    const PackingResult next = nextFitBinPacking(items, 10);
    std::cout << "First-Fit bins: ";
    for (int bin : first.binOfItem) std::cout << bin << ' ';
    std::cout << "\nNext-Fit bins: ";
    for (int bin : next.binOfItem) std::cout << bin << ' ';
    std::cout << '\n';
}
