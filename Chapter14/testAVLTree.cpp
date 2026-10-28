#include "avlTree.hpp"
#include <cassert>
#include <map>
#include <random>
#include <utility>
#include <vector>

static std::vector<int> keys(const avlTree<int, int>& tree) {
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
    const int rotations[][3] = {{30, 20, 10}, {10, 20, 30},
                                {30, 10, 20}, {10, 30, 20}};
    for (const auto& sequence : rotations) {
        avlTree<int, int> tree;
        for (int key : sequence) tree.insert({key, key * 10});
        assert(tree.valid() && tree.size() == 3 && tree.height() == 2);
        assert(keys(tree) == (std::vector<int>{10, 20, 30}));
    }
    avlTree<int, int> leftZero;
    for (int key : {4, 2, 5, 1, 3}) leftZero.insert({key, key});
    leftZero.erase(5); // 左孩子 BF=0；删除后仍应右单旋
    assert(leftZero.valid() && leftZero.height() == 3);
    avlTree<int, int> rightZero;
    for (int key : {2, 1, 4, 3, 5}) rightZero.insert({key, key});
    rightZero.erase(1); // 对称的右孩子 BF=0
    assert(rightZero.valid() && rightZero.height() == 3);

    avlTree<int, int> tree;
    dictionary<int, int>& adt = tree;
    assert(tree.empty() && tree.height() == 0 && adt.find(1) == nullptr);
    assert(tree.lowerBound(1) == nullptr);
    adt.erase(1);
    const int input[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
    for (int key : input) adt.insert({key, key});
    assert(tree.valid() && tree.size() == 9);
    adt.insert({6, 600});
    assert(tree.size() == 9 && tree.find(6)->second == 600);
    assert(tree.lowerBound(5)->first == 6 && tree.lowerBound(15) == nullptr);
    assert(tree.lowerBound(6)->first == 6);
    adt.erase(1);
    adt.erase(14);
    adt.erase(3);
    adt.erase(8);
    assert(tree.valid() && tree.size() == 5);
    assert(keys(tree) == (std::vector<int>{4, 6, 7, 10, 13}));
    adt.erase(999);
    assert(tree.size() == 5 && tree.valid());

    avlTree<int, int> successorCase;
    for (int key : {8, 4, 12, 10}) successorCase.insert({key, key});
    auto* successorBefore = successorCase.find(10);
    successorCase.erase(8);
    assert(successorCase.find(10) == successorBefore && successorCase.valid());

    avlTree<Key, Value, KeyLess> custom;
    custom.insert({Key(2), Value(20)});
    custom.insert({Key(1), Value(10)});
    custom.insert({Key(3), Value(30)});
    custom.insert({Key(2), Value(21)});
    assert(custom.valid() && custom.size() == 3);
    assert(custom.find(Key(2))->second.value == 21);

    avlTree<int, int> randomTree;
    std::map<int, int> reference;
    std::mt19937 rng(42);
    for (int step = 0; step < 1500; ++step) {
        const int key = static_cast<int>(rng() % 101);
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

    avlTree<int, int> ascending;
    for (int i = 0; i < 5000; ++i) ascending.insert({i, i});
    assert(ascending.valid() && ascending.size() == 5000 && ascending.height() <= 18);
    for (int i = 0; i < 5000; ++i) {
        ascending.erase(i);
        assert(ascending.valid());
    }
    assert(ascending.empty() && ascending.height() == 0);
    tree.clear();
    assert(tree.empty() && tree.height() == 0 && tree.valid());
}
