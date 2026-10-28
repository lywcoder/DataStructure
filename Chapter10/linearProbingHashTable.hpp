#ifndef CHAPTER10_LINEAR_PROBING_HASH_TABLE_HPP
#define CHAPTER10_LINEAR_PROBING_HASH_TABLE_HPP

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

// 开放寻址：数对直接放入槽位。冲突时探查 h(k),h(k)+1,...（对 m 取模）。
// 删除必须留下 Deleted 墓碑，否则会截断其他碰撞键的查找链。
template <class K, class E, class Hash = std::hash<K>,
          class KeyEqual = std::equal_to<K>>
class linearProbingHashTable : public dictionary<K, E> {
public:
    typedef std::pair<const K, E> value_type;

    explicit linearProbingHashTable(std::size_t initialSlots = 11,
                                    double maxLoadFactor = 0.7,
                                    const Hash& hash = Hash(),
                                    const KeyEqual& equal = KeyEqual())
        : slots_(checkedSlots(initialSlots)), states_(initialSlots, Empty),
          size_(0), deleted_(0), maximumLoad_(maxLoadFactor),
          hash_(hash), equal_(equal) {
        if (!(maxLoadFactor > 0.0 && maxLoadFactor < 1.0) ||
            !std::isfinite(maxLoadFactor))
            throw std::invalid_argument("linear probing load factor must be in (0,1)");
    }

    linearProbingHashTable(const linearProbingHashTable& other)
        : slots_(other.slots_.size()), states_(other.states_), size_(other.size_),
          deleted_(other.deleted_), maximumLoad_(other.maximumLoad_),
          hash_(other.hash_), equal_(other.equal_) {
        for (std::size_t i = 0; i < states_.size(); ++i)
            if (states_[i] == Occupied)
                slots_[i].reset(new value_type(*other.slots_[i]));
    }

    linearProbingHashTable& operator=(const linearProbingHashTable& other) {
        linearProbingHashTable copy(other);
        swap(copy);
        return *this;
    }

    bool empty() const override { return size_ == 0; }
    int size() const override { return size_; }
    std::size_t bucketCount() const { return slots_.size(); }
    std::size_t tombstones() const { return deleted_; }
    double loadFactor() const { return static_cast<double>(size_) / slots_.size(); }

    value_type* find(const K& key) const override {
        const std::size_t index = findIndex(key);
        return index == slots_.size() ? nullptr : slots_[index].get();
    }
    void erase(const K& key) override {
        const std::size_t index = findIndex(key);
        if (index == slots_.size()) return;
        slots_[index].reset();
        states_[index] = Deleted;
        --size_;
        ++deleted_;
    }
    void insert(const value_type& pair) override {
        const std::size_t existing = findIndex(pair.first);
        if (existing != slots_.size()) {
            slots_[existing]->second = pair.second;
            return;
        }
        if (size_ == std::numeric_limits<int>::max())
            throw std::length_error("dictionary size overflow");
        std::unique_ptr<value_type> fresh(new value_type(pair));
        prepareForInsertion();

        std::size_t index = hash_(fresh->first) % slots_.size();
        std::size_t firstDeleted = slots_.size();
        for (std::size_t step = 0; step < slots_.size(); ++step) {
            if (states_[index] == Deleted && firstDeleted == slots_.size())
                firstDeleted = index;
            if (states_[index] == Empty) {
                if (firstDeleted != slots_.size()) index = firstDeleted;
                break;
            }
            index = advance(index, slots_.size());
        }
        if (states_[index] == Occupied && firstDeleted != slots_.size())
            index = firstDeleted;
        if (states_[index] == Occupied)
            throw std::logic_error("no available hash slot");
        if (states_[index] == Deleted) --deleted_;
        slots_[index] = std::move(fresh);
        states_[index] = Occupied;
        ++size_;
    }

    // 先完成分配和所有散列计算，再移动独占指针；若 Hash 抛异常，旧表不变。
    // 数对本身不搬移，所以 rehash 后已有数对指针保持有效。
    void rehash(std::size_t requestedSlots) {
        checkedSlots(requestedSlots);
        std::size_t count = requestedSlots;
        while (static_cast<long double>(size_) / count > maximumLoad_) {
            if (count > slots_.max_size() / 2)
                throw std::length_error("hash table capacity overflow");
            count *= 2;
        }
        if (count > slots_.max_size())
            throw std::length_error("hash table capacity overflow");
        if (count == slots_.size() && deleted_ == 0) return;
        std::vector<std::unique_ptr<value_type>> fresh(count);
        std::vector<unsigned char> newStates(count, Empty);
        std::vector<std::pair<std::size_t, std::size_t>> positions;
        positions.reserve(static_cast<std::size_t>(size_));
        for (std::size_t old = 0; old < slots_.size(); ++old) {
            if (states_[old] != Occupied) continue;
            std::size_t target = hash_(slots_[old]->first) % count;
            while (newStates[target] == Occupied) target = advance(target, count);
            newStates[target] = Occupied;
            positions.push_back(std::make_pair(old, target));
        }
        for (const auto& position : positions)
            fresh[position.second] = std::move(slots_[position.first]);
        slots_.swap(fresh);
        states_.swap(newStates);
        deleted_ = 0;
    }

    void output(std::ostream& out) const {
        // 槽位遍历顺序不是键排序；rehash 后也可能变化。
        for (std::size_t i = 0; i < slots_.size(); ++i)
            if (states_[i] == Occupied)
                out << slots_[i]->first << ' ' << slots_[i]->second << '\n';
    }

private:
    enum State : unsigned char { Empty, Occupied, Deleted };
    static_assert(noexcept(std::swap(std::declval<Hash&>(), std::declval<Hash&>())),
                  "Hash must support non-throwing std::swap");
    static_assert(noexcept(std::swap(std::declval<KeyEqual&>(), std::declval<KeyEqual&>())),
                  "KeyEqual must support non-throwing std::swap");
    static std::size_t checkedSlots(std::size_t count) {
        if (count == 0) throw std::invalid_argument("slot count must be positive");
        return count;
    }
    static std::size_t advance(std::size_t index, std::size_t count) {
        return index + 1 == count ? 0 : index + 1;
    }
    std::size_t findIndex(const K& key) const {
        std::size_t index = hash_(key) % slots_.size();
        for (std::size_t step = 0; step < slots_.size(); ++step) {
            if (states_[index] == Empty) break;
            if (states_[index] == Occupied && equal_(slots_[index]->first, key))
                return index;
            index = advance(index, slots_.size());
        }
        return slots_.size();
    }
    void prepareForInsertion() {
        if (static_cast<long double>(size_ + 1) / slots_.size() > maximumLoad_) {
            std::size_t count = slots_.size();
            do {
                if (count > slots_.max_size() / 2)
                    throw std::length_error("hash table capacity overflow");
                count *= 2;
            } while (static_cast<long double>(size_ + 1) / count > maximumLoad_);
            rehash(count);
        } else if (deleted_ > slots_.size() / 4 ||
                   static_cast<std::size_t>(size_) + deleted_ + 1 >= slots_.size()) {
            rehash(slots_.size()); // 清理墓碑，缩短未来探查链。
        }
    }
    void swap(linearProbingHashTable& other) noexcept {
        using std::swap;
        swap(hash_, other.hash_);
        swap(equal_, other.equal_);
        slots_.swap(other.slots_);
        states_.swap(other.states_);
        swap(size_, other.size_);
        swap(deleted_, other.deleted_);
        swap(maximumLoad_, other.maximumLoad_);
    }
    std::vector<std::unique_ptr<value_type>> slots_;
    std::vector<unsigned char> states_;
    int size_;
    std::size_t deleted_;
    double maximumLoad_;
    Hash hash_;
    KeyEqual equal_;
};

template <class K, class E, class Hash, class KeyEqual>
std::ostream& operator<<(std::ostream& out,
    const linearProbingHashTable<K, E, Hash, KeyEqual>& table) {
    table.output(out);
    return out;
}

#endif
