#include "bracket.hpp"
#include "hanoi.hpp"
#include "linkedstack.hpp"
#include "maze.hpp"
#include "offlineEquivalence.hpp"
#include "railroad.hpp"
#include "switchBox.hpp"

#include <iostream>

int main() {
    arrayStack<int> arrayBased(2);
    linkedStack<int> linked;
    stack<int>& first = arrayBased;
    stack<int>& second = linked;
    first.push(10); first.push(20);
    second.push(10); second.push(20);
    std::cout << "stack tops: " << first.top() << " / " << second.top() << '\n';

    std::cout << "brackets in a{b[c](d)}:\n";
    printMatchedPairs("a{b[c](d)}", std::cout);

    std::cout << "Hanoi with 3 disks:\n";
    printHanoiMoves(3, std::cout);

    std::cout << "railroad [3,4,2,1], 2 holding tracks:\n";
    printRailroadMoves(rearrangeRailroad({3, 4, 2, 1}, 2), std::cout);

    std::cout << "switch box [1,2,2,1]: "
              << switchBoxRoutable({1, 2, 2, 1}) << '\n';

    const std::vector<std::vector<int>> groups = offlineEquivalenceClasses(
        6, {{0, 1}, {1, 2}, {3, 4}});
    std::cout << "offline equivalence classes:\n";
    for (const std::vector<int>& group : groups) {
        for (int element : group) std::cout << element << ' ';
        std::cout << '\n';
    }

    const std::vector<std::vector<int>> maze = {
        {0, 0, 1}, {1, 0, 1}, {1, 0, 0}
    };
    std::cout << "mouse path: ";
    for (const std::pair<int, int>& cell : findMazePath(maze, {0, 0}, {2, 2}))
        std::cout << '(' << cell.first << ',' << cell.second << ") ";
    std::cout << '\n';
}
