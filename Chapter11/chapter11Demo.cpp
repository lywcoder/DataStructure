#include "arrayBinaryTree.hpp"
#include "linkedBinaryTree.hpp"
#include "signalAmplifiers.hpp"
#include "disjointSet.hpp"
#include <iostream>
#include <vector>

int main() {
    linkedBinaryTree<int> linked;
    binaryTreeNode<int>* root = linked.setRoot(1);
    binaryTreeNode<int>* left = linked.insertLeft(root, 2);
    linked.insertRight(root, 3);
    linked.insertLeft(left, 4);
    linked.insertRight(left, 5);

    arrayBinaryTree<int> array;
    array.setRoot(1);
    array.insertLeft(1, 2);
    array.insertRight(1, 3);
    array.insertLeft(2, 4);
    array.insertRight(2, 5);

    binaryTree<int>* trees[2] = {&linked, &array};
    for (int i = 0; i < 2; ++i) {
        std::cout << (i == 0 ? "linked" : "array") << " preorder: ";
        trees[i]->preOrder([](int& x) { std::cout << x << ' '; });
        std::cout << "\nheight=" << trees[i]->height() << " size=" << trees[i]->size() << '\n';
    }

    linkedBinaryTree<signalNode> network;
    binaryTreeNode<signalNode>* source = network.setRoot(signalNode(0, 0));
    binaryTreeNode<signalNode>* branch = network.insertLeft(source, signalNode(1, 2));
    network.insertRight(source, signalNode(2, 1));
    network.insertLeft(branch, signalNode(3, 2));
    network.insertRight(branch, signalNode(4, 1));
    const std::vector<int> amplifiers = setSignalAmplifiers(network.rootNode(), 3);
    std::cout << "amplifiers at: ";
    for (int id : amplifiers) std::cout << id << ' ';
    std::cout << '\n';

    disjointSet groups(5);
    groups.unite(0, 1);
    groups.unite(3, 4);
    std::cout << "0 and 1 connected: " << groups.connected(0, 1)
              << ", components: " << groups.count() << '\n';
}
