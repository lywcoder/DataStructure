#include "maxheap.hpp"
#include "maxHBLT.hpp"
#include "heapSort.hpp"
#include "huffmanCoding.hpp"
#include "machineScheduling.hpp"
#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    maxHBLT<int> leftistA;
    maxHBLT<int> leftistB;
    std::priority_queue<int> referenceA;
    std::priority_queue<int> referenceB;
    std::mt19937 hbltRandom(137);
    for (int i = 0; i < 1000; ++i) {
        const int value = static_cast<int>(hbltRandom() % 201) - 100;
        if (i % 2 == 0) { leftistA.push(value); referenceA.push(value); }
        else { leftistB.push(value); referenceB.push(value); }
        if (i % 7 == 0 && !referenceA.empty()) {
            assert(leftistA.top() == referenceA.top());
            leftistA.pop(); referenceA.pop();
        }
        if (i % 11 == 0 && !referenceB.empty()) {
            assert(leftistB.top() == referenceB.top());
            leftistB.pop(); referenceB.pop();
        }
        assert(leftistA.valid() && leftistB.valid());
        assert((std::size_t(1) << leftistA.rootS()) <=
               static_cast<std::size_t>(leftistA.size()) + 1);
        assert((std::size_t(1) << leftistB.rootS()) <=
               static_cast<std::size_t>(leftistB.size()) + 1);
    }
    leftistA.merge(leftistB);
    while (!referenceB.empty()) { referenceA.push(referenceB.top()); referenceB.pop(); }
    assert(leftistB.empty() && leftistB.valid() && leftistA.valid());
    leftistA.merge(leftistA); // 与自身合并是空操作
    assert(leftistA.size() == static_cast<int>(referenceA.size()));
    while (!referenceA.empty()) {
        assert(leftistA.top() == referenceA.top());
        leftistA.pop(); referenceA.pop();
        assert(leftistA.valid());
    }
    assert(leftistA.empty() && leftistA.rootS() == 0);
    bool emptyHbltRejected = false;
    try { leftistA.pop(); } catch (const priorityQueueEmpty&) { emptyHbltRejected = true; }
    assert(emptyHbltRejected);
    const int hbltRaw[] = {4, 10, 3, 5, 1, 10};
    leftistA.initialize(hbltRaw, 6);
    assert(leftistA.valid() && leftistA.size() == 6 && leftistA.top() == 10);
    leftistA.initialize(std::vector<int>{2, 7, 6});
    assert(leftistA.valid() && leftistA.size() == 3 && leftistA.top() == 7);
    maxPriorityQueue<int>& leftistInterface = leftistA;
    leftistInterface.push(9);
    assert(leftistInterface.top() == 9 && leftistA.valid());
    leftistA.initialize(std::vector<int>());
    assert(leftistA.empty() && leftistA.valid());
    leftistA.merge(leftistB); // 两棵空树合并
    assert(leftistA.empty() && leftistB.empty());
    maxHBLT<int, std::greater<int> > minLeftist(std::vector<int>{4, 10, 3, 5, 1});
    assert(minLeftist.valid() && minLeftist.top() == 1);
    minLeftist.pop();
    assert(minLeftist.valid() && minLeftist.top() == 3);

    maxHeap<int> heap;
    bool emptyRejected = false;
    try { heap.top(); } catch (const priorityQueueEmpty&) { emptyRejected = true; }
    assert(emptyRejected);
    std::mt19937 random(42);
    std::priority_queue<int> reference;
    for (int i = 0; i < 1000; ++i) {
        if (reference.empty() || random() % 3 != 0) {
            const int value = static_cast<int>(random() % 201) - 100;
            heap.push(value);
            reference.push(value);
        } else {
            assert(heap.top() == reference.top());
            heap.pop(); reference.pop();
        }
        assert(heap.size() == static_cast<int>(reference.size()));
        if (!reference.empty()) assert(heap.top() == reference.top());
    }
    while (!reference.empty()) {
        assert(heap.top() == reference.top());
        heap.pop(); reference.pop();
    }
    assert(heap.empty());

    const int raw[] = {4, 10, 3, 5, 1, 10};
    heap.initialize(raw, 6);
    assert(heap.top() == 10 && heap.size() == 6);
    const int descending[] = {10, 10, 5, 4, 3, 1};
    for (int expectedValue : descending) {
        assert(heap.top() == expectedValue);
        heap.pop();
    }
    assert(heap.empty());
    maxHeap<int, std::greater<int> > minHeap(std::vector<int>{4, 10, 3, 5, 1});
    assert(minHeap.top() == 1);
    minHeap.pop();
    assert(minHeap.top() == 3);

    std::vector<int> values;
    for (int i = 0; i < 300; ++i) values.push_back(static_cast<int>(random() % 100));
    std::vector<int> expected = values;
    std::sort(expected.begin(), expected.end());
    heapSort(values);
    assert(values == expected);
    heapSort(values, std::greater<int>());
    std::sort(expected.begin(), expected.end(), std::greater<int>());
    assert(values == expected);
    std::vector<int> empty;
    heapSort(empty);
    assert(empty.empty());

    const std::map<unsigned char, std::uint64_t> frequencies{
        {'a', 5}, {'b', 9}, {'c', 12}, {'d', 13}, {'e', 16}, {'f', 45}};
    huffmanCoding coding(frequencies);
    assert(coding.weightedPathLength() == 224);
    const std::string message = "facebade";
    assert(coding.decode(coding.encode(message)) == message);
    for (const auto& a : coding.codes())
        for (const auto& b : coding.codes())
            if (a.first != b.first)
                assert(b.second.compare(0, a.second.size(), a.second) != 0);
    huffmanCoding one("aaaa");
    assert(one.encode("aaa") == "000" && one.decode("000") == "aaa");
    huffmanCoding none("");
    assert(none.encode("").empty() && none.decode("").empty());
    bool badBitsRejected = false;
    try { coding.decode("2"); } catch (const std::invalid_argument&) { badBitsRejected = true; }
    assert(badBitsRejected);
    bool unknownRejected = false;
    try { coding.encode("z"); } catch (const std::invalid_argument&) { unknownRejected = true; }
    assert(unknownRejected);

    const schedulingResult schedule = scheduleJobs(
        {{1, 7}, {2, 6}, {3, 5}, {4, 4}, {5, 3}, {6, 2}, {7, 1}}, 3);
    assert(schedule.assignments.size() == 7 && schedule.makespan == 10);
    std::vector<std::int64_t> machineEnd(3, 0);
    for (const schedulingAssignment& a : schedule.assignments) {
        assert(a.machineId >= 0 && a.machineId < 3);
        assert(a.start == machineEnd[a.machineId]);
        assert(a.finish >= a.start);
        machineEnd[a.machineId] = a.finish;
    }
    bool invalidScheduleRejected = false;
    try { scheduleJobs({{1, 2}}, 0); }
    catch (const std::invalid_argument&) { invalidScheduleRejected = true; }
    assert(invalidScheduleRejected);
}
