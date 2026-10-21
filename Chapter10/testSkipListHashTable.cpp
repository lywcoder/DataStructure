#include "skipList.hpp"
#include "hashTable.hpp"
// 重复包含应当安全；本文件单独编译为一个测试程序。
#include "skipList.hpp"
#include "hashTable.hpp"
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

struct ConstantHash {
    std::size_t operator()(int) const { return 0; }
};

template<class Action>
void expectInvalidArgument(Action action) {
    bool threw = false;
    try {
        action();
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
}

template<class Dictionary>
void testBasicOperations(Dictionary& data) {
    dictionary<int, std::string>& base = data;
    assert(base.empty());
    assert(base.size() == 0);
    assert(base.find(1) == nullptr);
    base.erase(1);
    base.insert({30, "thirty"});
    base.insert({10, "ten"});
    base.insert({20, "twenty"});
    base.insert({20, "updated"});
    assert(base.size() == 3);
    assert(base.find(20)->second == "updated");

    const Dictionary& view = data;
    view.find(20)->second = "modified"; // 遵循现有 dictionary 的 const 接口。
    assert(base.find(20)->second == "modified");
    base.erase(999);
    assert(base.size() == 3);

    Dictionary copy(data);
    assert(copy.size() == 3);
    assert(copy.find(20) != data.find(20));
    copy.erase(10);
    copy.insert({20, "copy"});
    copy.insert({40, "forty"});
    assert(data.find(10) != nullptr);
    assert(data.find(20)->second == "modified");
    assert(data.find(40) == nullptr);

    Dictionary assigned;
    assigned.insert({99, "old"});
    assigned = data;
    assert(assigned.find(99) == nullptr);
    assert(assigned.size() == 3);
    assert(assigned.find(20) != data.find(20));
    assigned.erase(30);
    assigned.find(20)->second = "assigned";
    assert(data.find(30) != nullptr);
    assert(data.find(20)->second == "modified");
    Dictionary& alias = assigned;
    assigned = alias;
    assert(assigned.size() == 2);
    assert(assigned.find(10)->second == "ten");
    assert(assigned.find(20)->second == "assigned");

    for (int key : {10, 20, 30})
        base.erase(key);
    assert(base.empty());
    assert(base.size() == 0);
    base.insert({5, "reuse"});
    assert(base.size() == 1);
    assert(base.find(5)->second == "reuse");

    Dictionary empty;
    Dictionary emptyCopy(empty);
    assert(emptyCopy.empty());
    assigned = empty;
    assert(assigned.empty());
    assert(assigned.find(20) == nullptr);
}

template<class Dictionary>
void checkAgainstMap(const Dictionary& data, const std::map<int, int>& expected,
                     bool orderedOutput) {
    assert(data.size() == static_cast<int>(expected.size()));
    assert(data.empty() == expected.empty());
    for (int key = -45; key <= 45; ++key) {
        auto actual = data.find(key);
        auto reference = expected.find(key);
        assert((actual == nullptr) == (reference == expected.end()));
        if (actual != nullptr) {
            assert(actual->first == key);
            assert(actual->second == reference->second);
        }
    }

    std::ostringstream actualOutput;
    actualOutput << data;
    std::ostringstream directOutput;
    data.output(directOutput);
    assert(actualOutput.str() == directOutput.str());
    if (orderedOutput) {
        std::ostringstream referenceOutput;
        for (const auto& entry : expected)
            referenceOutput << entry.first << ' ' << entry.second << '\n';
        assert(actualOutput.str() == referenceOutput.str());
    } else {
        // 哈希输出无序：比较内容，同时检测重复键或格式残留。
        std::istringstream input(actualOutput.str());
        std::map<int, int> parsed;
        for (std::size_t i = 0; i < expected.size(); ++i) {
            int key = 0;
            int value = 0;
            const bool read = static_cast<bool>(input >> key >> value);
            assert(read);
            const bool inserted = parsed.insert({key, value}).second;
            assert(inserted);
        }
        input >> std::ws;
        assert(input.eof());
        assert(parsed == expected);
    }
}

template<class Dictionary>
void testAgainstMap(Dictionary& data, bool orderedOutput) {
    std::map<int, int> expected;
    std::mt19937 random(42);
    std::uniform_int_distribution<int> keys(-40, 40);
    std::uniform_int_distribution<int> actions(0, 2);
    for (int step = 0; step < 1000; ++step) {
        const int key = keys(random);
        switch (actions(random)) {
        case 0:
            data.insert({key, step});
            expected[key] = step;
            break;
        case 1:
            data.erase(key);
            expected.erase(key);
            break;
        default: {
            auto found = data.find(key);
            assert((found == nullptr) == (expected.find(key) == expected.end()));
            break;
        }
        }
        checkAgainstMap(data, expected, orderedOutput);
    }
}

void testSkipListDetails() {
    skipList<int, int> data(12, 0.5, 42);
    assert(data.levels() == 1);
    data.insert({50, 500});
    auto saved = data.find(50);
    for (int key = 99; key >= 0; --key)
        data.insert({key, key * 10});
    assert(data.size() == 100);
    assert(data.find(50) == saved);
    assert(saved->second == 500);
    assert(data.levels() >= 1 && data.levels() <= 12);
    std::ostringstream layers;
    data.outputLevels(layers);
    assert(!layers.str().empty()); // 不绑定具体层标签或标准库的随机分布实现。
    for (int key = 0; key < 100; ++key)
        data.erase(key);
    assert(data.empty());
    assert(data.levels() == 1);
    data.insert({7, 70});
    assert(data.find(7)->second == 70);

    skipList<int, int> singleLevel(1, 0.5, 42);
    testAgainstMap(singleLevel, true);
    assert(singleLevel.levels() == 1);
}

void testCollisionDeletion() {
    // 从实际遍历顺序选择头/中/尾，兼容头插和尾插两种链表实现。
    for (int position = 0; position < 3; ++position) {
        hashTable<int, int, ConstantHash> data(11, 1.0);
        for (int key : {10, 20, 30})
            data.insert({key, key * 10});
        std::ostringstream output;
        output << data;
        std::istringstream input(output.str());
        int traversal[3] = {0, 0, 0};
        for (int i = 0; i < 3; ++i) {
            int value = 0;
            const bool read = static_cast<bool>(input >> traversal[i] >> value);
            assert(read);
            assert(value == traversal[i] * 10);
        }
        data.erase(traversal[position]);
        assert(data.size() == 2);
        for (int i = 0; i < 3; ++i) {
            if (i == position)
                assert(data.find(traversal[i]) == nullptr);
            else
                assert(data.find(traversal[i])->second == traversal[i] * 10);
        }
        data.erase(traversal[position]);
        assert(data.size() == 2);
        for (int key : traversal)
            data.erase(key);
        assert(data.empty());
        data.insert({99, 990});
        assert(data.find(99)->second == 990);
    }
}

void testHashGrowthAndRehash() {
    hashTable<int, int, ConstantHash> data(2, 0.5);
    assert(data.loadFactor() == 0.0);
    const std::size_t initialBuckets = data.bucketCount();
    data.insert({0, 0});
    auto saved = data.find(0);
    for (int key = 1; key <= 100; ++key) {
        data.insert({key, key * 10});
        assert(data.find(0) == saved);
        assert(data.loadFactor() <= 0.5 + 1e-12);
    }
    assert(data.bucketCount() > initialBuckets);
    assert(std::abs(data.loadFactor() -
                    static_cast<double>(data.size()) / data.bucketCount()) < 1e-12);
    const std::size_t beforeRehash = data.bucketCount();
    data.rehash(beforeRehash + 17);
    assert(data.bucketCount() >= beforeRehash + 17);
    assert(data.find(0) == saved);

    data.rehash(1); // 请求太小时必须向上调整，不能破坏负载因子约束。
    assert(static_cast<double>(data.bucketCount()) * 0.5 >= data.size());
    assert(data.loadFactor() <= 0.5 + 1e-12);
    assert(data.find(0) == saved);
    for (int key = 0; key <= 100; ++key)
        assert(data.find(key)->second == key * 10);
    const std::size_t buckets = data.bucketCount();
    expectInvalidArgument([&data]() { data.rehash(0); });
    assert(data.bucketCount() == buckets);
    assert(data.size() == 101);
    assert(data.find(0) == saved);

    hashTable<int, int> empty;
    empty.rehash(1);
    assert(empty.empty());
    assert(empty.bucketCount() >= 1);
    assert(empty.loadFactor() == 0.0);
}

// 均无默认构造函数；Key 只提供 <，没有 == 或 std::hash 特化。
struct Key {
    explicit Key(int value) : value(value) {}
    int value;
    bool operator<(const Key& other) const { return value < other.value; }
};

struct Value {
    explicit Value(int value) : value(value) {}
    int value;
};

struct KeyHash {
    std::size_t operator()(const Key& key) const {
        return std::hash<int>()(key.value);
    }
};

struct KeyEqual {
    bool operator()(const Key& left, const Key& right) const {
        return left.value == right.value;
    }
};

template<class Dictionary>
void testNonDefaultTypes(Dictionary& data) {
    data.insert({Key(2), Value(20)});
    data.insert({Key(1), Value(10)});
    data.insert({Key(2), Value(200)});
    assert(data.size() == 2);
    assert(data.find(Key(2))->second.value == 200);
    Dictionary copy(data);
    Dictionary assigned;
    assigned = data;
    data.erase(Key(1));
    assert(data.find(Key(1)) == nullptr);
    assert(copy.find(Key(1))->second.value == 10);
    assigned.find(Key(2))->second.value = 999;
    assert(data.find(Key(2))->second.value == 200);
}

struct ThrowingValue {
    static int live;
    static int copiesRemaining;
    int value;

    explicit ThrowingValue(int value) : value(value) { ++live; }
    ThrowingValue(const ThrowingValue& other) : value(other.value) {
        if (copiesRemaining == 0)
            throw std::runtime_error("Value copy failed");
        if (copiesRemaining > 0)
            --copiesRemaining;
        ++live;
    }
    ThrowingValue& operator=(const ThrowingValue& other) {
        value = other.value;
        return *this;
    }
    ~ThrowingValue() { --live; }
};

int ThrowingValue::live = 0;
int ThrowingValue::copiesRemaining = -1;

template<class Dictionary>
void testCopyFailureCleanup() {
    {
        Dictionary source;
        for (int key = 0; key < 5; ++key)
            source.insert({key, ThrowingValue(key)});
        Dictionary target;
        target.insert({99, ThrowingValue(99)});
        const int baseline = ThrowingValue::live;

        bool threw = false;
        ThrowingValue::copiesRemaining = 2;
        try {
            Dictionary copy(source);
        } catch (const std::runtime_error&) {
            threw = true;
        }
        ThrowingValue::copiesRemaining = -1;
        assert(threw);
        assert(ThrowingValue::live == baseline);

        threw = false;
        ThrowingValue::copiesRemaining = 2;
        try {
            target = source;
        } catch (const std::runtime_error&) {
            threw = true;
        }
        ThrowingValue::copiesRemaining = -1;
        assert(threw);
        assert(ThrowingValue::live == baseline);
        assert(target.size() == 1);
        assert(target.find(99)->second.value == 99);
        assert(source.size() == 5);
    }
    assert(ThrowingValue::live == 0);
}

struct ThrowingHash {
    int* callsRemaining;
    std::size_t operator()(int key) const {
        if (*callsRemaining == 0)
            throw std::runtime_error("Hash calculation failed");
        if (*callsRemaining > 0)
            --*callsRemaining;
        return std::hash<int>()(key);
    }
};

void testRehashFailure() {
    int callsRemaining = -1;
    hashTable<int, int, ThrowingHash> data(11, 0.75, ThrowingHash{&callsRemaining});
    for (int key = 0; key < 5; ++key)
        data.insert({key, key * 10});
    auto saved = data.find(2);
    std::ostringstream before;
    before << data;
    const std::size_t oldBuckets = data.bucketCount();

    callsRemaining = 2; // 让部分哈希计算成功，然后抛异常。
    bool threw = false;
    try {
        data.rehash(31);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    callsRemaining = -1;
    assert(threw);
    assert(data.bucketCount() == oldBuckets);
    assert(data.size() == 5);
    assert(data.find(2) == saved);
    std::ostringstream after;
    after << data;
    assert(before.str() == after.str());
    for (int key = 0; key < 5; ++key)
        assert(data.find(key)->second == key * 10);
}

void testInvalidParameters() {
    for (int level : {0, -1}) {
        expectInvalidArgument([level]() {
            skipList<int, int> invalid(level, 0.5, 42);
        });
    }
    for (double probability : {0.0, 1.0, -0.1, 1.1,
                              std::numeric_limits<double>::infinity(),
                              std::numeric_limits<double>::quiet_NaN()}) {
        expectInvalidArgument([probability]() {
            skipList<int, int> invalid(8, probability, 42);
        });
    }
    expectInvalidArgument([]() { hashTable<int, int> invalid(0); });
    for (double load : {0.0, -1.0,
                        std::numeric_limits<double>::infinity(),
                        std::numeric_limits<double>::quiet_NaN()}) {
        expectInvalidArgument([load]() { hashTable<int, int> invalid(11, load); });
    }
    // 最大负载因子并不要求 <= 1，链地址法允许一个桶含多个结点。
    hashTable<int, int> valid(1, 2.0);
    valid.insert({1, 10});
    assert(valid.find(1)->second == 10);
}

} // namespace

int main() {
    skipList<int, std::string> skipBasic(16, 0.5, 42);
    hashTable<int, std::string> hashBasic;
    testBasicOperations(skipBasic);
    testBasicOperations(hashBasic);
    hashTable<int, std::string, ConstantHash> collisionBasic;
    testBasicOperations(collisionBasic);

    skipList<int, int> skipRandom(16, 0.5, 42);
    hashTable<int, int> hashRandom;
    hashTable<int, int, ConstantHash> collisionRandom(3, 0.75);
    testAgainstMap(skipRandom, true);
    testAgainstMap(hashRandom, false);
    testAgainstMap(collisionRandom, false);
    testSkipListDetails();
    testCollisionDeletion();
    testHashGrowthAndRehash();
    testInvalidParameters();
    testCopyFailureCleanup<skipList<int, ThrowingValue>>();
    testCopyFailureCleanup<hashTable<int, ThrowingValue>>();
    testRehashFailure();

    skipList<Key, Value> skipObjects(8, 0.5, 42);
    hashTable<Key, Value, KeyHash, KeyEqual> hashObjects;
    testNonDefaultTypes(skipObjects);
    testNonDefaultTypes(hashObjects);
    std::cout << "All skip list and hash table tests passed.\n";
}
