#include "hashFunctions.hpp"
#include "linearProbingHashTable.hpp"
#include "lzw.hpp"

#include <cassert>
#include <functional>
#include <limits>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct ConstantHash {
    std::size_t operator()(int) const { return 0; }
};

template <class Table>
void compareWithMap(Table& table) {
    std::map<int, int> expected;
    std::mt19937 rng(2026);
    for (int step = 0; step < 1500; ++step) {
        const int key = static_cast<int>(rng() % 91) - 45;
        if (rng() % 3 == 0) {
            table.erase(key);
            expected.erase(key);
        } else {
            table.insert({key, step});
            expected[key] = step;
        }
        assert(table.size() == static_cast<int>(expected.size()));
        for (int probe = -48; probe <= 48; ++probe) {
            const auto found = table.find(probe);
            const auto reference = expected.find(probe);
            assert((found == nullptr) == (reference == expected.end()));
            if (found) assert(found->second == reference->second);
        }
        assert(table.loadFactor() < 0.8);
    }
}

void testLinearProbing() {
    linearProbingHashTable<int, std::string, ConstantHash> table(11, 0.7);
    dictionary<int, std::string>& base = table;
    base.insert({10, "ten"});
    base.insert({20, "twenty"});
    base.insert({30, "thirty"});
    assert(base.contains(20) && base.size() == 3);
    base.erase(20); // 后续碰撞键 30 仍必须跨墓碑被找到。
    assert(!base.contains(20) && base.find(30)->second == "thirty");
    assert(table.tombstones() == 1);
    base.insert({40, "forty"}); // 复用墓碑。
    assert(table.tombstones() == 0 && base.find(40)->second == "forty");
    base.insert({10, "TEN"});
    assert(base.size() == 3 && base.find(10)->second == "TEN");
    const auto saved = base.find(10);
    for (int k = 50; k < 150; ++k) table.insert({k, std::to_string(k)});
    assert(base.find(10) == saved); // rehash 只移动独占指针，不移动数对。
    table.rehash(257);
    assert(table.tombstones() == 0 && table.find(10) == saved);

    linearProbingHashTable<int, std::string, ConstantHash> copy(table);
    copy.erase(10);
    assert(table.find(10) != nullptr && copy.find(10) == nullptr);
    linearProbingHashTable<int, std::string, ConstantHash> assigned;
    assigned = table;
    assigned = assigned;
    assert(assigned.find(30)->second == "thirty");
    std::ostringstream out;
    out << assigned;
    assert(out.str().find("10 TEN") != std::string::npos);

    linearProbingHashTable<int, int> one(1, 0.7);
    one.insert({1, 10});
    assert(one.bucketCount() > 1 && one.find(1)->second == 10);
    bool threw = false;
    try { linearProbingHashTable<int, int> invalid(0); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { linearProbingHashTable<int, int> invalid(11, 1.0); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { linearProbingHashTable<int, int> invalid(
        11, std::numeric_limits<double>::quiet_NaN()); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { table.rehash(0); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    linearProbingHashTable<int, int> randomTable(3, 0.75);
    compareWithMap(randomTable);
    linearProbingHashTable<int, int, ConstantHash> collisions(3, 0.75);
    compareWithMap(collisions);
}

struct Key {
    int value;
    explicit Key(int x) : value(x) {}
};
struct Value {
    int value;
    explicit Value(int x) : value(x) {}
};
struct KeyHash {
    std::size_t operator()(const Key& key) const { return static_cast<std::size_t>(key.value); }
};
struct KeyEqual {
    bool operator()(const Key& a, const Key& b) const { return a.value == b.value; }
};

void testNonDefaultTypes() {
    linearProbingHashTable<Key, Value, KeyHash, KeyEqual> table;
    table.insert({Key(2), Value(20)});
    table.insert({Key(1), Value(10)});
    table.insert({Key(2), Value(200)});
    assert(table.size() == 2 && table.find(Key(2))->second.value == 200);
    table.erase(Key(1));
    assert(table.find(Key(1)) == nullptr);
}

struct ThrowingHash {
    int* remaining;
    std::size_t operator()(int value) const {
        if (*remaining == 0) throw std::runtime_error("hash failed");
        if (*remaining > 0) --*remaining;
        return std::hash<int>()(value);
    }
};

void testRehashFailure() {
    int remaining = -1;
    linearProbingHashTable<int, int, ThrowingHash> table(
        11, 0.7, ThrowingHash{&remaining});
    for (int key = 0; key < 5; ++key) table.insert({key, key * 10});
    const auto saved = table.find(2);
    const std::size_t oldSlots = table.bucketCount();
    remaining = 2;
    bool threw = false;
    try { table.rehash(31); }
    catch (const std::runtime_error&) { threw = true; }
    remaining = -1;
    assert(threw && table.bucketCount() == oldSlots && table.find(2) == saved);
    for (int key = 0; key < 5; ++key)
        assert(table.find(key)->second == key * 10);
}

void testLzw() {
    assert(lzwCompress("").empty());
    assert(lzwDecompress({}).empty());
    const std::string special = "ABABABA";
    const std::vector<int> specialCodes = lzwCompress(special);
    assert((specialCodes == std::vector<int>{65, 66, 256, 258}));
    assert(lzwDecompress(specialCodes) == special); // 触发 KwKwK 特例。
    assert(lzwDecompress(lzwCompress("TOBEORNOTTOBEORTOBEORNOT")) ==
           "TOBEORNOTTOBEORTOBEORNOT");

    std::string repeated;
    for (int i = 0; i < 800; ++i) repeated += "AB";
    for (int limit : {256, 257, 258, 259, 4096})
        assert(lzwDecompress(lzwCompress(repeated, limit), limit) == repeated);

    std::mt19937 rng(42);
    for (int limit : {256, 257, 512, 4096}) {
        for (int trial = 0; trial < 120; ++trial) {
            std::string input;
            const int length = static_cast<int>(rng() % 200);
            for (int i = 0; i < length; ++i)
                input.push_back(static_cast<char>(rng() % 256));
            const std::vector<int> codes = lzwCompress(input, limit);
            assert(lzwDecompress(codes, limit) == input);
            for (int code : codes) assert(code >= 0 && code < limit);
        }
    }
    std::string allBytes;
    for (int c = 0; c < 256; ++c) allBytes.push_back(static_cast<char>(c));
    assert(lzwDecompress(lzwCompress(allBytes)) == allBytes);

    bool threw = false;
    try { lzwDecompress({256}); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { lzwDecompress({65, 9999}); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { lzwCompress("a", 255); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
}

int main() {
    testLinearProbing();
    testNonDefaultTypes();
    testRehashFailure();
    testLzw();
}
