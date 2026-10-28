#include "binarySearchTree.hpp"
#include <algorithm>
#include <cassert>
#include <map>
#include <random>
#include <string>
#include <utility>
#include <vector>

static std::vector<int> keys(const binarySearchTree<int, int>& tree) {
    std::vector<int> result;
    tree.inOrder([&result](const std::pair<const int, int>& item) {
        result.push_back(item.first);
    });
    return result;
}

struct Key {
    int value;
    explicit Key(int n) : value(n) {}
};
struct KeyLess {
    bool operator()(const Key& a, const Key& b) const { return a.value < b.value; }
};
struct Value {
    int value;
    explicit Value(int n) : value(n) {}
};
struct Tracked {
    static int live;
    int value;
    explicit Tracked(int n) : value(n) { ++live; }
    Tracked(const Tracked& other) : value(other.value) { ++live; }
    Tracked& operator=(const Tracked& other) { value = other.value; return *this; }
    ~Tracked() { --live; }
};
int Tracked::live = 0;

int main() {
    binarySearchTree<int, int> tree;
    assert(tree.empty() && tree.size() == 0 && tree.height() == 0);
    assert(tree.find(1) == nullptr);
    tree.erase(1);
    const int initial[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
    for (int key : initial) tree.insert({key, key * 10});
    assert(tree.size() == 9 && tree.height() == 4);
    assert(keys(tree) == (std::vector<int>{1, 3, 4, 6, 7, 8, 10, 13, 14}));
    assert(tree.find(6) && tree.find(6)->second == 60);
    tree.insert({6, 600});
    assert(tree.size() == 9 && tree.find(6)->second == 600);
    tree.erase(1);  // 叶子
    assert(keys(tree) == (std::vector<int>{3, 4, 6, 7, 8, 10, 13, 14}));
    tree.erase(14); // 只有左孩子 13
    assert(keys(tree) == (std::vector<int>{3, 4, 6, 7, 8, 10, 13}));
    tree.erase(3);  // 两个孩子，后继在右子树内部
    assert(keys(tree) == (std::vector<int>{4, 6, 7, 8, 10, 13}));
    tree.erase(8);  // 删除根，后继是直接右孩子
    assert(keys(tree) == (std::vector<int>{4, 6, 7, 10, 13}));
    assert(tree.size() == 5 && tree.height() == 3);
    tree.erase(999);
    assert(tree.size() == 5);

    binarySearchTree<Key, Value, KeyLess> custom;
    custom.insert({Key(2), Value(20)});
    custom.insert({Key(1), Value(10)});
    custom.insert({Key(2), Value(21)});
    assert(custom.size() == 2 && custom.find(Key(2))->second.value == 21);
    custom.erase(Key(1));
    assert(custom.size() == 1);

    binarySearchTree<int, int> randomTree;
    dictionary<int, int>& abstractDictionary = randomTree;
    std::map<int, int> reference;
    std::mt19937 engine(42);
    for (int i = 0; i < 1500; ++i) {
        const int key = static_cast<int>(engine() % 101);
        if (engine() % 3 == 0) {
            abstractDictionary.erase(key);
            reference.erase(key);
        } else {
            abstractDictionary.insert({key, i});
            reference[key] = i;
        }
        assert(abstractDictionary.size() == static_cast<int>(reference.size()));
        for (int probe = 0; probe < 101; ++probe) {
            const auto it = reference.find(probe);
            const auto* item = abstractDictionary.find(probe);
            assert((item == nullptr) == (it == reference.end()));
            if (item) assert(item->second == it->second);
        }
        std::vector<int> expected;
        for (const auto& item : reference) expected.push_back(item.first);
        assert(keys(randomTree) == expected);
    }
    randomTree.clear();
    assert(randomTree.empty() && randomTree.height() == 0);

    binarySearchTree<int, int> skewed;
    for (int i = 0; i < 1000; ++i) skewed.insert({i, i});
    assert(skewed.height() == 1000); // 普通 BST 不自动平衡
    skewed.clear();
    assert(skewed.empty());

    {
        binarySearchTree<int, Tracked> owned;
        for (int i = 0; i < 1000; ++i)
            owned.insert({static_cast<int>(engine() % 2000), Tracked(i)});
        owned.erase(500);
        owned.clear();
        assert(owned.empty() && Tracked::live == 0);
    }
    assert(Tracked::live == 0);
}
