#include "binarySearchTree.hpp"
#include "indexedBinarySearchTree.hpp"
#include "histogram.hpp"
#include "bestFitBinPacking.hpp"
#include "crossingDistribution.hpp"
#include <iostream>
#include <string>
#include <utility>

int main() {
    binarySearchTree<int, std::string> students;
    const int keys[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
    for (int key : keys) students.insert({key, "student-" + std::to_string(key)});

    std::cout << "size=" << students.size() << " height=" << students.height() << '\n';
    const auto* found = students.find(6);
    if (found) std::cout << "find 6: " << found->second << '\n';
    students.insert({6, "updated"}); // 已有键：更新值
    students.erase(1);                 // 叶子
    students.erase(14);                // 一个孩子
    students.erase(8);                 // 两个孩子，删除根
    std::cout << "in-order keys: ";
    students.inOrder([](const std::pair<const int, std::string>& item) {
        std::cout << item.first << ' ';
    });
    std::cout << "\nsize=" << students.size() << " height=" << students.height() << '\n';

    indexedBinarySearchTree<int, std::string> indexed;
    for (int key : keys) indexed.insert({key, "student-" + std::to_string(key)});
    std::cout << "index 3 key=" << indexed.get(3)->first
              << ", rank(7)=" << indexed.rank(7) << '\n';
    indexed.eraseAt(3);
    std::cout << "after eraseAt(3), index 3 key=" << indexed.get(3)->first << '\n';

    std::cout << "histogram: ";
    for (const auto& item : histogram(std::vector<int>{2, 4, 2, 2, 3, 4, 2, 6, 4, 2}))
        std::cout << item.first << ':' << item.second << ' ';
    std::cout << '\n';

    const BinPackingResult packing = bestFitBinPacking({4, 8, 1, 4, 2, 1}, 10);
    std::cout << "best-fit bin of each item: ";
    for (int bin : packing.binOfItem) std::cout << bin << ' ';
    std::cout << '\n';

    const CrossingDistribution crossings =
        crossingDistribution({8, 7, 4, 2, 5, 1, 9, 3, 10, 6});
    std::cout << "crossings per wire: ";
    for (int count : crossings.perWire) std::cout << count << ' ';
    std::cout << "(total " << crossings.total << ")\n";

}
