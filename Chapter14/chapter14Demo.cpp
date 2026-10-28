#include "binarySearchTree.hpp"
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
}
