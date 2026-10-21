#include "binSort.hpp"
#include "circularList.hpp"
#include "convexHull.hpp"
#include "disjointSet.hpp"
#include "radixSort.hpp"

#include <algorithm>
#include <cassert>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

template <class List>
void checkListBasics() {
    List list;
    assert(list.empty() && list.size() == 0);
    list.insert(0, 2);
    list.insert(0, 1);
    list.insert(2, 4);
    list.insert(2, 3);
    assert(list.size() == 4 && list.indexOf(3) == 2);
    list.erase(0);
    list.erase(list.size() - 1);
    assert(list.size() == 2 && list.at(0) == 2 && list.at(1) == 3);

    List copied(list);
    copied.at(0) = 99;
    assert(list.at(0) == 2 && copied.at(0) == 99);
    List assigned;
    assigned = list;
    assigned = assigned;
    assert(assigned.at(1) == 3);

    std::ostringstream out;
    out << list;
    assert(out.str() == "2 3");

    bool threw = false;
    try { list.erase(2); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);
    threw = false;
    try { list.insert(-1, 5); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);
}

void checkChainAndBuckets() {
    checkListBasics<chain<int>>();
    chain<int> values(1);
    for (int x : {4, 1, 3, 1, 0, 2}) values.push_back(x);
    values.binSort(4);
    const int expected[] = {0, 1, 1, 2, 3, 4};
    int i = 0;
    for (int x : values) assert(x == expected[i++]);
    assert(i == 6);
    const chain<int>& constValues = values;
    assert(*constValues.begin() == 0);

    chain<int> invalid;
    invalid.push_back(1);
    invalid.push_back(7);
    bool threw = false;
    try { invalid.binSort(3); } catch (const std::out_of_range&) { threw = true; }
    assert(threw && invalid.at(0) == 1 && invalid.at(1) == 7);

    chain<studentRecord> students;
    students.push_back(studentRecord(2, "A"));
    students.push_back(studentRecord(1, "B"));
    students.push_back(studentRecord(2, "C"));
    students.push_back(studentRecord(0, "D"));
    binSort(students, 2);
    assert(students.at(0).name == "D" && students.at(1).name == "B");
    assert(students.at(2).name == "A" && students.at(3).name == "C");
}

void checkCircularAndDouble() {
    checkListBasics<circularListWithHeader<int>>();
    checkListBasics<doublyLinkedList<int>>();

    circularListWithHeader<int> circular;
    extendedLinearList<int>& base = circular;
    base.push_back(4);
    base.push_back(5);
    base.clear();
    assert(circular.empty());
    circular.push_back(6);
    assert(circular.at(0) == 6);
    circular.erase(0);
    assert(circular.empty());

    doublyLinkedList<int> doubled;
    doubled.push_back(1);
    doubled.push_back(2);
    doubled.push_back(3);
    assert(doubled.front() == 1 && doubled.back() == 3);
    assert(doubled.secondBack() == 2);
    doubled.pop_back();
    assert(doubled.back() == 2);
    doubled.erase(0);
    doubled.pop_back();
    assert(doubled.empty());
}

template <class List>
void compareListWithVector() {
    List list;
    std::vector<int> expected;
    std::mt19937 rng(2026);
    for (int step = 0; step < 1500; ++step) {
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

void checkRadixSort() {
    chain<int> values;
    std::vector<int> expected;
    std::mt19937 rng(42);
    for (int i = 0; i < 1000; ++i) {
        const int x = static_cast<int>(rng() % 100000);
        values.push_back(x);
        expected.push_back(x);
    }
    std::sort(expected.begin(), expected.end());
    radixSort(values);
    int i = 0;
    for (int x : values) assert(x == expected[static_cast<std::size_t>(i++)]);
    assert(i == 1000);

    chain<int> binary;
    for (int x : {7, 0, 3, 7, 1}) binary.push_back(x);
    radixSort(binary, 2);
    const int answer[] = {0, 1, 3, 7, 7};
    for (int j = 0; j < 5; ++j) assert(binary.at(j) == answer[j]);

    chain<int> invalid;
    invalid.push_back(2);
    invalid.push_back(-1);
    bool threw = false;
    try { radixSort(invalid); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw && invalid.at(0) == 2 && invalid.at(1) == -1);
}

void checkHull() {
    chain<Point> points;
    for (const Point& p : {Point(1, 1), Point(0, 0), Point(2, 0),
                           Point(2, 2), Point(0, 2), Point(1, 0),
                           Point(0, 0), Point(2, 1)})
        points.push_back(p);
    const chain<Point> hull = convexHull(points);
    const Point corners[] = {Point(0, 0), Point(2, 0), Point(2, 2), Point(0, 2)};
    assert(hull.size() == 4);
    for (int i = 0; i < 4; ++i) assert(hull.at(i) == corners[i]);
    assert(points.size() == 8); // 不修改输入。

    chain<Point> line;
    for (const Point& p : {Point(2, 2), Point(0, 0), Point(1, 1), Point(1, 1)})
        line.push_back(p);
    const chain<Point> endpoints = convexHull(line);
    assert(endpoints.size() == 2);
    assert(endpoints.at(0) == Point(0, 0));
    assert(endpoints.at(1) == Point(2, 2));

    chain<Point> empty;
    assert(convexHull(empty).empty());
    empty.push_back(Point(3, 4));
    assert(convexHull(empty).size() == 1);
}

void checkDisjointSet() {
    disjointSet sets(6);
    assert(sets.components() == 6);
    assert(sets.unite(0, 1));
    assert(sets.unite(1, 2));
    assert(!sets.unite(0, 2));
    assert(sets.unite(4, 5));
    assert(sets.connected(0, 2) && !sets.connected(0, 5));
    assert(sets.componentSize(1) == 3 && sets.components() == 3);
    assert(sets.unite(2, 5));
    assert(sets.componentSize(4) == 5 && sets.components() == 2);
    bool threw = false;
    try { sets.find(6); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);
    disjointSet empty(0);
    assert(empty.components() == 0);

    // 用朴素的集合标签作独立参照，检查合并返回值和连通性。
    disjointSet randomSets(20);
    std::vector<int> label(20);
    for (int i = 0; i < 20; ++i) label[static_cast<std::size_t>(i)] = i;
    std::mt19937 rng(7);
    for (int step = 0; step < 500; ++step) {
        const int a = static_cast<int>(rng() % 20);
        const int b = static_cast<int>(rng() % 20);
        const bool wereSeparate = label[static_cast<std::size_t>(a)] !=
                                  label[static_cast<std::size_t>(b)];
        assert(randomSets.unite(a, b) == wereSeparate);
        const int oldLabel = label[static_cast<std::size_t>(b)];
        const int newLabel = label[static_cast<std::size_t>(a)];
        for (int& value : label)
            if (value == oldLabel) value = newLabel;
        for (int i = 0; i < 20; ++i)
            for (int j = 0; j < 20; ++j)
                assert(randomSets.connected(i, j) ==
                       (label[static_cast<std::size_t>(i)] ==
                        label[static_cast<std::size_t>(j)]));
    }
}

int main() {
    checkChainAndBuckets();
    checkCircularAndDouble();
    compareListWithVector<chain<int>>();
    compareListWithVector<circularListWithHeader<int>>();
    compareListWithVector<doublyLinkedList<int>>();
    checkRadixSort();
    checkHull();
    checkDisjointSet();
}
