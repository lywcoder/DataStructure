#include "bTree.hpp"
#include <cassert>
#include <functional>
#include <map>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

static std::vector<int> keys(const bTree<int, int>& tree) {
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
    bool badOrder = false;
    try { bTree<int, int> invalid(2); }
    catch (const std::invalid_argument&) { badOrder = true; }
    assert(badOrder);

    for (int order : {3, 4, 5, 6, 7}) {
        bTree<int, int> tree(order);
        dictionary<int, int>& adt = tree;
        assert(tree.empty() && tree.valid() && tree.height() == 0);
        assert(tree.lowerBound(1) == nullptr && adt.find(1) == nullptr);
        adt.erase(1);
        for (int key = 0; key < 200; ++key) {
            adt.insert({key, key * 10});
            assert(tree.valid());
        }
        assert(tree.size() == 200 && tree.height() > 1);
        assert(tree.find(73)->second == 730);
        assert(tree.lowerBound(73)->first == 73);
        assert(tree.lowerBound(74)->first == 74);
        assert(tree.lowerBound(200) == nullptr);
        adt.insert({73, 999});
        assert(tree.size() == 200 && tree.find(73)->second == 999 && tree.valid());
        for (int key = 199; key >= 0; --key) {
            adt.erase(key);
            assert(tree.valid() && tree.size() == key);
        }
        assert(tree.empty() && tree.height() == 0);
    }

    bTree<Key, Value, KeyLess> custom(5);
    custom.insert({Key(2), Value(20)});
    custom.insert({Key(1), Value(10)});
    custom.insert({Key(3), Value(30)});
    custom.insert({Key(2), Value(21)});
    assert(custom.valid() && custom.size() == 3);
    assert(custom.find(Key(2))->second.value == 21);
    custom.erase(Key(2));
    assert(custom.valid() && custom.size() == 2);

    bTree<int, int, std::greater<int> > descending(4);
    for (int key : {1, 3, 2, 5, 4}) descending.insert({key, key});
    assert(descending.valid() && descending.find(3)->second == 3);
    assert(descending.lowerBound(3)->first == 3);

    std::mt19937 rng(1515);
    for (int order : {3, 4, 5, 8}) {
        bTree<int, int> tree(order);
        std::map<int, int> reference;
        for (int step = 0; step < 3000; ++step) {
            const int key = static_cast<int>(rng() % 211);
            if (step % 3 == 0) { tree.erase(key); reference.erase(key); }
            else { tree.insert({key, step}); reference[key] = step; }
            assert(tree.valid() && tree.size() == static_cast<int>(reference.size()));
            std::vector<int> expected;
            for (const auto& item : reference) {
                expected.push_back(item.first);
                assert(tree.find(item.first)->second == item.second);
            }
            assert(keys(tree) == expected);
            const int probe = static_cast<int>(rng() % 211);
            const auto expectedNext = reference.lower_bound(probe);
            const auto* actualNext = tree.lowerBound(probe);
            assert((expectedNext == reference.end()) == (actualNext == nullptr));
            if (actualNext) assert(actualNext->first == expectedNext->first);
        }
        tree.clear();
        assert(tree.empty() && tree.valid());
    }
}
