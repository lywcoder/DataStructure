#include "indexedBinarySearchTree.hpp"
#include <algorithm>
#include <cassert>
#include <map>
#include <random>
#include <utility>
#include <vector>

struct NoDefaultValue {
    int value;
    explicit NoDefaultValue(int x) : value(x) {}
};

int main() {
    indexedBinarySearchTree<int, NoDefaultValue> tree;
    dictionary<int, NoDefaultValue>& adt = tree;
    assert(tree.empty() && tree.get(0) == nullptr);
    const int input[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
    for (int key : input) adt.insert({key, NoDefaultValue(key * 10)});
    const int sorted[] = {1, 3, 4, 6, 7, 8, 10, 13, 14};
    for (int i = 0; i < 9; ++i) {
        assert(tree.get(i) && tree.get(i)->first == sorted[i]);
        assert(tree.rank(sorted[i]) == i);
    }
    assert(tree.size() == 9 && tree.height() == 4);
    assert(tree.get(-1) == nullptr && tree.get(9) == nullptr);
    assert(tree.rank(5) == 3 && tree.rank(100) == 9);
    assert(tree.lowerBound(5)->first == 6 && tree.lowerBound(15) == nullptr);
    adt.insert({6, NoDefaultValue(600)});
    assert(tree.size() == 9 && tree.get(3)->second.value == 600);
    adt.erase(3);                     // 两个孩子，后继在右子树内部
    assert(tree.eraseAt(0));          // 叶子 1
    assert(tree.eraseAt(6));          // 原 14，只有左孩子
    adt.erase(8);                     // 两个孩子，后继是直接右孩子
    const int after[] = {4, 6, 7, 10, 13};
    assert(tree.size() == 5 && tree.height() == 4);
    for (int i = 0; i < 5; ++i) assert(tree.get(i)->first == after[i]);
    assert(!tree.eraseAt(-1) && !tree.eraseAt(5));

    indexedBinarySearchTree<int, int> randomTree;
    std::map<int, int> reference;
    std::mt19937 rng(42);
    for (int step = 0; step < 1500; ++step) {
        int key = static_cast<int>(rng() % 101);
        if (step % 4 == 0) {
            randomTree.erase(key);
            reference.erase(key);
        } else if (step % 7 == 0 && !reference.empty()) {
            const int index = static_cast<int>(rng() % reference.size());
            auto it = reference.begin();
            std::advance(it, index);
            assert(randomTree.eraseAt(index));
            reference.erase(it);
        } else {
            randomTree.insert({key, step});
            reference[key] = step;
        }
        assert(randomTree.size() == static_cast<int>(reference.size()));
        int index = 0;
        for (const auto& item : reference) {
            assert(randomTree.get(index)->first == item.first);
            assert(randomTree.get(index)->second == item.second);
            assert(randomTree.rank(item.first) == index);
            ++index;
        }
        assert(randomTree.get(index) == nullptr);
    }
    randomTree.clear();
    assert(randomTree.empty() && randomTree.height() == 0);

    indexedBinarySearchTree<int, int> skewed;
    for (int i = 0; i < 20000; ++i) skewed.insert({i, i});
    assert(skewed.size() == 20000 && skewed.get(19999)->first == 19999);
    skewed.erase(19999);
    assert(skewed.size() == 19999 && skewed.get(19998)->first == 19998);
    assert(skewed.eraseAt(19998) && skewed.size() == 19998);
    skewed.clear();
    assert(skewed.empty());
}
