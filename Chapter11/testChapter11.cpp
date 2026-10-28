#include "arrayBinaryTree.hpp"
#include "linkedBinaryTree.hpp"
#include "signalAmplifiers.hpp"
#include "disjointSet.hpp"
#include <cassert>
#include <stdexcept>
#include <vector>

struct noDefault {
    int value;
    explicit noDefault(int x) : value(x) {}
};

static std::vector<int> order(binaryTree<int>& tree, int which) {
    std::vector<int> result;
    binaryTree<int>::visitor collect = [&result](int& x) { result.push_back(x); };
    if (which == 0) tree.preOrder(collect);
    if (which == 1) tree.inOrder(collect);
    if (which == 2) tree.postOrder(collect);
    if (which == 3) tree.levelOrder(collect);
    return result;
}

static void checkOrders(binaryTree<int>& tree) {
    assert(tree.size() == 5 && tree.height() == 3);
    assert(order(tree, 0) == (std::vector<int>{1, 2, 4, 5, 3}));
    assert(order(tree, 1) == (std::vector<int>{4, 2, 5, 1, 3}));
    assert(order(tree, 2) == (std::vector<int>{4, 5, 2, 3, 1}));
    assert(order(tree, 3) == (std::vector<int>{1, 2, 3, 4, 5}));
}

int main() {
    linkedBinaryTree<int> linked;
    assert(linked.empty() && linked.height() == 0);
    binaryTreeNode<int>* root = linked.setRoot(1);
    binaryTreeNode<int>* left = linked.insertLeft(root, 2);
    linked.insertRight(root, 3);
    linked.insertLeft(left, 4);
    linked.insertRight(left, 5);
    checkOrders(linked);
    linked.eraseLeft(root);
    assert(linked.size() == 2 && linked.height() == 2);
    linked.clear();
    assert(linked.empty() && linked.height() == 0);

    arrayBinaryTree<int> array;
    array.setRoot(1);
    array.insertLeft(1, 2);
    array.insertRight(1, 3);
    array.insertLeft(2, 4);
    array.insertRight(2, 5);
    checkOrders(array);
    array.eraseLeft(1);
    assert(array.size() == 2 && array.at(4) == nullptr);
    array.clear();
    assert(array.empty() && array.height() == 0);

    arrayBinaryTree<noDefault> sparse;
    sparse.setRoot(noDefault(7));
    std::size_t child = sparse.insertRight(1, noDefault(8));
    child = sparse.insertRight(child, noDefault(9));
    assert(child == 7 && sparse.size() == 3 && sparse.height() == 3);
    assert(sparse.slotCount() == 7 && sparse.at(2) == nullptr);

    linkedBinaryTree<signalNode> network;
    binaryTreeNode<signalNode>* source = network.setRoot(signalNode(0, 0));
    binaryTreeNode<signalNode>* branch = network.insertLeft(source, signalNode(1, 2));
    network.insertLeft(branch, signalNode(2, 2));
    network.insertRight(source, signalNode(3, 1));
    assert(setSignalAmplifiers(network.rootNode(), 3) == (std::vector<int>{1}));
    bool rejected = false;
    try { setSignalAmplifiers(network.rootNode(), 1); }
    catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);

    disjointSet groups(5);
    assert(groups.count() == 5);
    assert(groups.unite(0, 1));
    assert(groups.unite(2, 3));
    assert(groups.unite(1, 2));
    assert(!groups.unite(0, 3));
    assert(groups.connected(0, 3) && !groups.connected(0, 4));
    assert(groups.componentSize(2) == 4 && groups.count() == 2);
}
