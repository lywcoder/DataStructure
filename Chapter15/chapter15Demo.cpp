#include "avlTree.hpp"
#include "redBlackTree.hpp"
#include "bTree.hpp"
#include <iostream>
#include <string>

int main() {
    avlTree<int, std::string> tree;
    for (int key = 1; key <= 15; ++key)
        tree.insert({key, "value-" + std::to_string(key)});
    std::cout << "AVL sorted insertion: size=" << tree.size()
              << " height=" << tree.height() << '\n';
    tree.erase(8);
    std::cout << "AVL after erase(8): height=" << tree.height()
              << " valid=" << tree.valid() << '\n';

    redBlackTree<int, std::string> redBlack;
    for (int key = 1; key <= 15; ++key)
        redBlack.insert({key, "value-" + std::to_string(key)});
    std::cout << "Red-black sorted insertion: size=" << redBlack.size()
              << " height=" << redBlack.height() << '\n';
    std::cout << "Red-black find(8): " << redBlack.find(8)->second << '\n';
    redBlack.erase(8);
    std::cout << "Red-black after erase(8): height=" << redBlack.height()
              << " valid=" << redBlack.valid() << '\n';

    bTree<int, std::string> multiway(5); // 5 阶：每结点最多 4 个键
    for (int key = 1; key <= 15; ++key)
        multiway.insert({key, "value-" + std::to_string(key)});
    std::cout << "B-tree (order 5) sorted insertion: size=" << multiway.size()
              << " height=" << multiway.height() << '\n';
    std::cout << "B-tree find(8): " << multiway.find(8)->second << '\n';
    multiway.erase(8);
    std::cout << "B-tree after erase(8): height=" << multiway.height()
              << " valid=" << multiway.valid() << '\n';
}
