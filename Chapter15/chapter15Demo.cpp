#include "avlTree.hpp"
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
}
