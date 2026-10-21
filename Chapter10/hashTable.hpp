#ifndef hashTable_
#define hashTable_

#include "dictionary.hpp"
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

// 链地址法：桶数组中的每个位置指向一条处理散列冲突的单链表。
// Hash 和 KeyEqual 必须满足：相等的键具有相同的散列值。
template<class K, class E, class Hash = std::hash<K>,
         class KeyEqual = std::equal_to<K>>
class hashTable : public dictionary<K, E> {
public:
    typedef std::pair<const K, E> value_type;

    explicit hashTable(std::size_t initialBuckets = 11, double maxLoadFactor = 0.75,
                       const Hash& hash = Hash(), const KeyEqual& equal = KeyEqual());
    hashTable(const hashTable<K, E, Hash, KeyEqual>& other);
    hashTable<K, E, Hash, KeyEqual>& operator=(
        const hashTable<K, E, Hash, KeyEqual>& other);
    ~hashTable() override;

    bool empty() const override { return listSize == 0; }
    int size() const override { return listSize; }
    std::size_t bucketCount() const { return buckets.size(); }
    double loadFactor() const { return static_cast<double>(listSize) / buckets.size(); }
    value_type* find(const K& theKey) const override;
    void insert(const value_type& thePair) override;
    void erase(const K& theKey) override;
    void rehash(std::size_t newBucketCount);
    void output(std::ostream& out) const;

private:
    // 复制赋值必须同时提交策略和桶；交换策略不能在中途抛异常。
    static_assert(noexcept(std::swap(std::declval<Hash&>(), std::declval<Hash&>())),
                  "Hash must support non-throwing std::swap");
    static_assert(noexcept(std::swap(std::declval<KeyEqual&>(), std::declval<KeyEqual&>())),
                  "KeyEqual must support non-throwing std::swap");

    struct node {
        value_type element;
        node* next;
        node(const value_type& thePair, node* nextNode = nullptr)
            : element(thePair), next(nextNode) {}
    };

    static std::size_t checkedBucketCount(std::size_t count);
    void swap(hashTable<K, E, Hash, KeyEqual>& other) noexcept;

    std::vector<node*> buckets;
    int listSize;
    double maximumLoadFactor;
    Hash hashFunction;
    KeyEqual keyEqual;
};

template<class K, class E, class Hash, class KeyEqual>
std::size_t hashTable<K, E, Hash, KeyEqual>::checkedBucketCount(std::size_t count) {
    if (count == 0)
        throw std::invalid_argument("Hash table bucket count must be > 0");
    return count;
}

template<class K, class E, class Hash, class KeyEqual>
hashTable<K, E, Hash, KeyEqual>::hashTable(
    std::size_t initialBuckets, double maxLoadFactor,
    const Hash& hash, const KeyEqual& equal)
    : buckets(checkedBucketCount(initialBuckets), nullptr), listSize(0),
      maximumLoadFactor(maxLoadFactor), hashFunction(hash), keyEqual(equal) {
    if (!(maxLoadFactor > 0.0) || !std::isfinite(maxLoadFactor))
        throw std::invalid_argument("Hash table max load factor must be finite and > 0");
}

template<class K, class E, class Hash, class KeyEqual>
hashTable<K, E, Hash, KeyEqual>::hashTable(
    const hashTable<K, E, Hash, KeyEqual>& other)
    : hashTable(other.buckets.size(), other.maximumLoadFactor,
                other.hashFunction, other.keyEqual) {
    // 使用临时表管理已复制结点，防止中途复制失败造成内存泄漏。
    hashTable<K, E, Hash, KeyEqual> copy(
        other.buckets.size(), other.maximumLoadFactor,
        other.hashFunction, other.keyEqual);
    for (std::size_t i = 0; i < buckets.size(); ++i) {
        node** tail = &copy.buckets[i];
        for (node* current = other.buckets[i]; current != nullptr;
             current = current->next) {
            *tail = new node(current->element);
            tail = &(*tail)->next;
            ++copy.listSize;
        }
    }
    buckets.swap(copy.buckets);
    listSize = copy.listSize;
    copy.listSize = 0;
}

template<class K, class E, class Hash, class KeyEqual>
hashTable<K, E, Hash, KeyEqual>& hashTable<K, E, Hash, KeyEqual>::operator=(
    const hashTable<K, E, Hash, KeyEqual>& other) {
    if (this != &other) {
        hashTable<K, E, Hash, KeyEqual> copy(other);
        swap(copy);
    }
    return *this;
}

template<class K, class E, class Hash, class KeyEqual>
void hashTable<K, E, Hash, KeyEqual>::swap(
    hashTable<K, E, Hash, KeyEqual>& other) noexcept {
    std::swap(hashFunction, other.hashFunction);
    std::swap(keyEqual, other.keyEqual);
    buckets.swap(other.buckets);
    std::swap(listSize, other.listSize);
    std::swap(maximumLoadFactor, other.maximumLoadFactor);
}

template<class K, class E, class Hash, class KeyEqual>
hashTable<K, E, Hash, KeyEqual>::~hashTable() {
    for (std::size_t i = 0; i < buckets.size(); ++i) {
        node* current = buckets[i];
        while (current != nullptr) {
            node* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }
}

template<class K, class E, class Hash, class KeyEqual>
typename hashTable<K, E, Hash, KeyEqual>::value_type*
hashTable<K, E, Hash, KeyEqual>::find(const K& theKey) const {
    std::size_t index = hashFunction(theKey) % buckets.size();
    for (node* current = buckets[index]; current != nullptr; current = current->next)
        if (keyEqual(current->element.first, theKey))
            return &current->element;
    return nullptr;
}

template<class K, class E, class Hash, class KeyEqual>
void hashTable<K, E, Hash, KeyEqual>::insert(const value_type& thePair) {
    value_type* existing = find(thePair.first);
    if (existing != nullptr) {
        existing->second = thePair.second;
        return;
    }
    if (listSize == std::numeric_limits<int>::max())
        throw std::length_error("Hash table size is too large");

    std::unique_ptr<node> newNode(new node(thePair));
    if (static_cast<double>(listSize + 1) / buckets.size() > maximumLoadFactor) {
        if (buckets.size() > buckets.max_size() / 2)
            throw std::length_error("Hash table bucket count is too large");
        // 极小装载因子可能要求超过一次翻倍的容量。
        long double minimum = std::ceil(
            static_cast<long double>(listSize + 1) / maximumLoadFactor);
        if (minimum > static_cast<long double>(buckets.max_size()))
            throw std::length_error("Hash table bucket count is too large");
        std::size_t count = buckets.size() * 2;
        if (minimum > static_cast<long double>(count))
            count = static_cast<std::size_t>(minimum);
        rehash(count);
    }

    std::size_t index = hashFunction(newNode->element.first) % buckets.size();
    newNode->next = buckets[index];
    buckets[index] = newNode.release();
    ++listSize;
}

template<class K, class E, class Hash, class KeyEqual>
void hashTable<K, E, Hash, KeyEqual>::erase(const K& theKey) {
    std::size_t index = hashFunction(theKey) % buckets.size();
    node* previous = nullptr;
    node* current = buckets[index];
    while (current != nullptr && !keyEqual(current->element.first, theKey)) {
        previous = current;
        current = current->next;
    }
    if (current == nullptr)
        return;
    if (previous == nullptr)
        buckets[index] = current->next;
    else
        previous->next = current->next;
    delete current;
    --listSize;
}

template<class K, class E, class Hash, class KeyEqual>
void hashTable<K, E, Hash, KeyEqual>::rehash(std::size_t newBucketCount) {
    checkedBucketCount(newBucketCount);
    long double minimum = std::ceil(
        static_cast<long double>(listSize) / maximumLoadFactor);
    if (minimum > static_cast<long double>(buckets.max_size()))
        throw std::length_error("Hash table bucket count is too large");
    if (minimum > static_cast<long double>(newBucketCount))
        newBucketCount = static_cast<std::size_t>(minimum);
    if (newBucketCount == buckets.size())
        return;

    std::vector<node*> newBuckets(newBucketCount, nullptr);
    std::vector<std::pair<node*, std::size_t>> positions;
    positions.reserve(listSize);
    // 先完成所有分配和散列计算，避免 Hash 抛异常时已破坏旧链表。
    for (std::size_t i = 0; i < buckets.size(); ++i)
        for (node* current = buckets[i]; current != nullptr; current = current->next)
            positions.push_back({current, hashFunction(current->element.first) % newBucketCount});

    for (const auto& position : positions) {
        node* current = position.first;
        std::size_t index = position.second;
        current->next = newBuckets[index];
        newBuckets[index] = current;
    }
    buckets.swap(newBuckets);
}

template<class K, class E, class Hash, class KeyEqual>
void hashTable<K, E, Hash, KeyEqual>::output(std::ostream& out) const {
    // 桶遍历顺序不等于键的大小顺序，扩容后输出顺序可能改变。
    for (std::size_t i = 0; i < buckets.size(); ++i)
        for (node* current = buckets[i]; current != nullptr; current = current->next)
            out << current->element.first << ' ' << current->element.second << '\n';
}

template<class K, class E, class Hash, class KeyEqual>
std::ostream& operator<<(std::ostream& out, const hashTable<K, E, Hash, KeyEqual>& x) {
    x.output(out);
    return out;
}

#endif
