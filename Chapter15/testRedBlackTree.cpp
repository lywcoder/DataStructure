#include "redBlackTree.hpp"
#include <cassert>
#include <map>
#include <random>
#include <utility>
#include <vector>

static std::vector<int> keys(const redBlackTree<int, int>& tree) {
    std::vector<int> result;
    tree.inOrder([&result](const std::pair<const int, int>& item) {
        result.push_back(item.first);
    });
    return result;
}

struct Key { int value; explicit Key(int x) : value(x) {} };
struct KeyLess {
    bool operator()(const Key& a, const Key& b) const { return a.value < b.value; }
};
struct Value { int value; explicit Value(int x) : value(x) {} };

int main() {
    redBlackTree<int, int> tree;
    dictionary<int, int>& adt = tree;
    assert(tree.empty() && tree.valid() && tree.height() == 0);
    assert(adt.find(1) == nullptr && tree.lowerBound(1) == nullptr);
    adt.erase(1);
    for (int key : {8, 3, 10, 1, 6, 14, 4, 7, 13}) {
        adt.insert({key, key * 10});
        assert(tree.valid());
    }
    assert(tree.size() == 9 && tree.find(6)->second == 60);
    adt.insert({6, 600});
    assert(tree.size() == 9 && tree.find(6)->second == 600 && tree.valid());
    assert(tree.lowerBound(5)->first == 6 && tree.lowerBound(15) == nullptr);
    for (int key : {1, 14, 3, 8, 999, 6, 4, 7, 10, 13}) {
        adt.erase(key);
        assert(tree.valid());
    }
    assert(tree.empty() && tree.size() == 0);

    // 叶子、单孩子、双孩子、根的删除；后继结点的地址必须保持稳定。
    redBlackTree<int, int> successor;
    for (int key : {8, 4, 12, 10, 14, 9, 11}) successor.insert({key, key});
    auto* before = successor.find(9);
    successor.erase(8);
    assert(successor.valid() && successor.find(9) == before);

    redBlackTree<Key, Value, KeyLess> custom;
    custom.insert({Key(2), Value(20)});
    custom.insert({Key(1), Value(10)});
    custom.insert({Key(3), Value(30)});
    custom.insert({Key(2), Value(21)});
    assert(custom.valid() && custom.size() == 3);
    assert(custom.find(Key(2))->second.value == 21);
    custom.erase(Key(2));
    assert(custom.valid() && custom.size() == 2);

    redBlackTree<int, int> randomTree;
    std::map<int, int> reference;
    std::mt19937 rng(1543);
    for (int step = 0; step < 3000; ++step) {
        const int key = static_cast<int>(rng() % 211);
        if (step % 3 == 0) { randomTree.erase(key); reference.erase(key); }
        else { randomTree.insert({key, step}); reference[key] = step; }
        assert(randomTree.valid() && randomTree.size() == static_cast<int>(reference.size()));
        std::vector<int> expected;
        for (const auto& item : reference) {
            expected.push_back(item.first);
            assert(randomTree.find(item.first)->second == item.second);
        }
        assert(keys(randomTree) == expected);
    }

    redBlackTree<int, int> ascending;
    for (int i = 0; i < 5000; ++i) ascending.insert({i, i});
    assert(ascending.valid() && ascending.height() <= 2 * 13);
    for (int i = 0; i < 5000; ++i) {
        ascending.erase(i);
        assert(ascending.valid());
    }
    assert(ascending.empty() && ascending.height() == 0);
    randomTree.clear();
    assert(randomTree.empty() && randomTree.valid());
}
