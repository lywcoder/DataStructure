#include "arrayList.hpp"
#include "vector.hpp"

#include <algorithm>
#include <cassert>
#include <ostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

template <class List>
void checkBasicOperations() {
    List list(2);
    assert(list.empty() && list.size() == 0 && list.capacity() >= 2);
    list.insert(0, 10);
    list.insert(1, 30);
    list.insert(1, 20);
    list.insert(3, 20);
    assert(list.size() == 4 && list.indexOf(20) == 1);
    assert(list.at(0) == 10 && list.at(1) == 20);
    assert(list.at(2) == 30 && list.at(3) == 20);

    std::ostringstream out;
    out << list;
    assert(out.str() == "10 20 30 20");

    const int aliasedValue = list.at(2);
    list.insert(0, list.at(2)); // 从自身取值，插入时可能移动或扩容。
    assert(list.at(0) == aliasedValue);
    list.erase(0);
    list.erase(1);
    assert(list.size() == 3 && list.at(0) == 10 && list.at(1) == 30);

    linearList<int>& asAbstractList = list;
    assert(asAbstractList.size() == 3 && asAbstractList.get(1) == 30);

    const List& constList = list;
    assert(constList.at(2) == 20);

    bool threw = false;
    try { list.at(-1); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);
    threw = false;
    try { list.erase(list.size()); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);
    threw = false;
    try { list.insert(list.size() + 1, 7); }
    catch (const std::out_of_range&) { threw = true; }
    assert(threw);

    std::sort(list.begin(), list.end());
    assert(list.at(0) == 10 && list.at(1) == 20 && list.at(2) == 30);

    List spareCapacity(8);
    spareCapacity.insert(0, 1);
    spareCapacity.insert(1, 2);
    spareCapacity.insert(2, 3);
    spareCapacity.insert(0, spareCapacity.at(1)); // 不扩容时也须先保存值。
    assert(spareCapacity.at(0) == 2 && spareCapacity.at(2) == 2);
}

template <class List>
void compareWithVector() {
    List list(1);
    std::vector<int> expected;
    std::mt19937 rng(2026);
    for (int step = 0; step < 3000; ++step) {
        const unsigned int action = rng() % 4;
        if (expected.empty() || action < 2) {
            const int index = static_cast<int>(rng() % (expected.size() + 1));
            const int value = static_cast<int>(rng() % 101) - 50;
            list.insert(index, value);
            expected.insert(expected.begin() + index, value);
        } else if (action == 2) {
            const int index = static_cast<int>(rng() % expected.size());
            list.erase(index);
            expected.erase(expected.begin() + index);
        } else {
            const int index = static_cast<int>(rng() % expected.size());
            const int value = static_cast<int>(rng() % 101) - 50;
            list.at(index) = value;
            expected[static_cast<std::size_t>(index)] = value;
        }
        assert(list.size() == static_cast<int>(expected.size()));
        for (int i = 0; i < list.size(); ++i)
            assert(list.at(i) == expected[static_cast<std::size_t>(i)]);
    }
}

struct NoDefault {
    int value;
    explicit NoDefault(int x) : value(x) {}
    bool operator==(const NoDefault& other) const { return value == other.value; }
    friend std::ostream& operator<<(std::ostream& out, const NoDefault& x) {
        return out << x.value;
    }
};

int main() {
    bool threw = false;
    try { arrayList<int> invalid(0); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { vectorList<int> invalid(0); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    checkBasicOperations<arrayList<int>>();
    checkBasicOperations<vectorList<int>>();
    compareWithVector<arrayList<int>>();
    compareWithVector<vectorList<int>>();

    arrayList<int> original(1);
    original.insert(0, 5);
    original.insert(1, 9);
    arrayList<int> copied(original);
    copied.at(0) = 42;
    assert(original.at(0) == 5 && copied.at(0) == 42);
    arrayList<int> assigned;
    assigned = original;
    assigned = assigned;
    assigned.erase(0);
    assert(original.size() == 2 && assigned.size() == 1);

    vectorList<NoDefault> objects(1);
    objects.insert(0, NoDefault(7));
    objects.insert(1, NoDefault(9));
    assert(objects.indexOf(NoDefault(9)) == 1);
    vectorList<NoDefault> objectsCopy(objects);
    objectsCopy.at(0).value = 11;
    assert(objects.at(0).value == 7);
    objects.erase(0);
    assert(objects.at(0).value == 9);
}
