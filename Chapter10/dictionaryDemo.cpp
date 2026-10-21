#include "dictionary.hpp"
#include "sortedArrayList.hpp"
#include "sortedChain.hpp"
#include "skipList.hpp"
#include "hashTable.hpp"
#include <cstddef>
#include <iostream>
#include <string>

namespace {

// 故意让所有键碰撞，观察链地址法仍然可以区分不同的键。
struct ConstantHash {
    std::size_t operator()(int) const { return 0; }
};

void exerciseDictionary(dictionary<int, std::string>& data) {
    data.insert({30, "thirty"});
    data.insert({10, "ten"});
    data.insert({20, "twenty"});
    data.insert({20, "TWENTY"});
    std::cout << "size after updating key 20: " << data.size() << '\n';
    if (auto entry = data.find(20))
        std::cout << "find(20): " << entry->second << '\n';
    data.erase(10);
    data.erase(999); // 删除不存在的键不改变字典。
    std::cout << "find(10) after erase: "
              << (data.find(10) == nullptr ? "not found" : "found") << '\n';
}

} // namespace

int main() {
    sortedArrayList<int, std::string> array;
    sortedChain<int, std::string> chain;
    skipList<int, std::string> skip(8, 0.5, 42);
    hashTable<int, std::string, ConstantHash> hash(3, 0.75);

    std::cout << "=== Sorted array ===\n";
    exerciseDictionary(array);
    std::cout << array;

    std::cout << "\n=== Sorted chain ===\n";
    exerciseDictionary(chain);
    std::cout << chain;

    std::cout << "\n=== Skip list (seed = 42) ===\n";
    exerciseDictionary(skip);
    for (int key : {5, 15, 25, 35, 40, 50})
        skip.insert({key, std::to_string(key)});
    std::cout << "Bottom-level traversal:\n" << skip;
    std::cout << "Active levels: " << skip.levels() << '\n';
    skip.outputLevels(std::cout);

    std::cout << "\n=== Chained hash table (all keys collide) ===\n";
    std::cout << "Before inserts: buckets=" << hash.bucketCount()
              << ", load=" << hash.loadFactor() << '\n';
    exerciseDictionary(hash);
    std::cout << "After inserts/erase: buckets=" << hash.bucketCount()
              << ", load=" << hash.loadFactor() << '\n';
    std::cout << "Bucket traversal (not sorted):\n" << hash;
    auto saved = hash.find(20);
    hash.rehash(23);
    std::cout << "After rehash: buckets=" << hash.bucketCount()
              << ", load=" << hash.loadFactor() << '\n';
    std::cout << "Pointer to key 20 is stable: " << std::boolalpha
              << (saved == hash.find(20)) << '\n';
}
