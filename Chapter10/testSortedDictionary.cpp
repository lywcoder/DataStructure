#include "sortedArrayList.hpp"
#include "sortedChain.hpp"
// 重复包含也不应产生重定义。
#include "dictionary.hpp"
#include "sortedArrayList.hpp"
#include "sortedChain.hpp"
#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

template<class Dictionary>
void testBasicOperations() {
    Dictionary data;
    dictionary<int, std::string>& base = data;
    assert(base.empty());
    assert(base.size() == 0);
    assert(base.find(10) == nullptr);
    base.erase(10);

    base.insert({30, "thirty"});
    base.insert({10, "ten"});
    base.insert({20, "twenty"});
    base.insert({40, "forty"});
    assert(base.size() == 4);
    assert(base.find(10)->second == "ten");
    assert(base.find(20)->second == "twenty");
    assert(base.find(40)->second == "forty");
    assert(base.find(5) == nullptr);
    assert(base.find(25) == nullptr);
    assert(base.find(50) == nullptr);

    std::ostringstream out;
    out << data;
    assert(out.str() == "10 ten\n20 twenty\n30 thirty\n40 forty\n");

    base.insert({20, "updated"});
    assert(base.size() == 4);
    assert(base.find(20)->second == "updated");
    base.find(20)->second = "modified";
    assert(base.find(20)->second == "modified");

    Dictionary copy(data);
    copy.erase(10);
    copy.insert({20, "copy"});
    assert(base.find(10) != nullptr);
    assert(base.find(20)->second == "modified");

    Dictionary assigned;
    assigned.insert({99, "old"});
    assigned = data;
    assigned.erase(40);
    assert(assigned.find(99) == nullptr);
    assert(base.find(40) != nullptr);
    Dictionary& same = assigned;
    assigned = same;
    assert(assigned.size() == 3);

    base.erase(10); // 队首位置
    base.erase(30); // 中间位置
    base.erase(40); // 尾部位置
    base.erase(999);
    assert(base.size() == 1);
    base.erase(20); // 最后一个元素
    assert(base.empty());
    base.insert({5, "reuse"});
    assert(base.size() == 1);
    assert(base.find(5)->second == "reuse");

    Dictionary empty;
    Dictionary emptyCopy(empty);
    assert(emptyCopy.empty());
    assigned = empty;
    assert(assigned.empty());
}

template<class Dictionary>
void testAgainstMap() {
    Dictionary data;
    std::map<int, int> expected;
    std::mt19937 random(42);
    std::uniform_int_distribution<int> keys(-40, 40);
    std::uniform_int_distribution<int> actions(0, 2);

    for (int i = 0; i < 1000; ++i) {
        int key = keys(random);
        if (actions(random) == 0) {
            data.erase(key);
            expected.erase(key);
        } else {
            data.insert({key, i});
            expected[key] = i;
        }
        assert(data.size() == static_cast<int>(expected.size()));
        assert(data.empty() == expected.empty());
        for (int probe = -45; probe <= 45; ++probe) {
            auto found = data.find(probe);
            auto reference = expected.find(probe);
            assert((found == nullptr) == (reference == expected.end()));
            if (found != nullptr)
                assert(found->second == reference->second);
        }

        std::ostringstream actualOutput;
        std::ostringstream expectedOutput;
        actualOutput << data;
        for (const auto& entry : expected)
            expectedOutput << entry.first << ' ' << entry.second << '\n';
        assert(actualOutput.str() == expectedOutput.str());
    }
}

// 键和值均没有默认构造函数，且键只提供 <，不提供 ==。
struct Key {
    explicit Key(int value) : value(value) {}
    int value;
    bool operator<(const Key& other) const { return value < other.value; }
};

struct Value {
    explicit Value(int value) : value(value) {}
    int value;
};

template<class Dictionary>
void testNonDefaultTypes() {
    Dictionary data;
    data.insert({Key(2), Value(20)});
    data.insert({Key(1), Value(10)});
    data.insert({Key(2), Value(200)});
    assert(data.size() == 2);
    assert(data.find(Key(2))->second.value == 200);
    Dictionary copy(data);
    data.erase(Key(1));
    assert(copy.find(Key(1))->second.value == 10);
    assert(data.find(Key(1)) == nullptr);
}

int main() {
    testBasicOperations<sortedArrayList<int, std::string>>();
    testBasicOperations<sortedChain<int, std::string>>();
    testAgainstMap<sortedArrayList<int, int>>();
    testAgainstMap<sortedChain<int, int>>();
    testNonDefaultTypes<sortedArrayList<Key, Value>>();
    testNonDefaultTypes<sortedChain<Key, Value>>();

    sortedArrayList<int, std::string> small(1);
    for (int i = 50; i >= 0; --i)
        small.insert({i, std::to_string(i)});
    assert(small.size() == 51);
    assert(small.capacity() >= small.size());
    for (int i = 0; i <= 50; ++i)
        assert(small.find(i)->second == std::to_string(i));

    for (int capacity : {0, -1}) {
        bool threw = false;
        try {
            sortedArrayList<int, int> invalid(capacity);
        } catch (const std::invalid_argument&) {
            threw = true;
        }
        assert(threw);
    }
    std::cout << "All sorted dictionary tests passed.\n";
}
