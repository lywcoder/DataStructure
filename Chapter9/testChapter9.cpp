#include "arrayQueue.hpp"
#include "circuitRouting.hpp"
#include "factorySimulation.hpp"
#include "imageComponents.hpp"
#include "railroad.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <deque>
#include <queue>
#include <random>
#include <stdexcept>
#include <vector>

template <class Queue>
void checkQueue() {
    Queue values;
    assert(values.empty() && values.size() == 0);
    bool threw = false;
    try { values.front(); } catch (const queueEmpty&) { threw = true; }
    assert(threw);
    values.push(10);
    values.push(20);
    assert(values.front() == 10 && values.back() == 20);
    values.push(values.front()); // 入队值引用队内元素。
    assert(values.back() == 10);
    values.pop();
    assert(values.front() == 20);
    Queue copy(values);
    copy.pop();
    assert(copy.front() == 10 && values.front() == 20);
    Queue assigned;
    assigned = values;
    assigned = assigned;
    assert(assigned.front() == 20 && assigned.back() == 10);
    const Queue& readOnly = values;
    assert(readOnly.front() == 20 && readOnly.back() == 10);
    queue<int>& asADT = values;
    asADT.pop();
    asADT.pop();
    assert(asADT.empty());
    asADT.push(7); // 空队列复用。
    assert(asADT.front() == 7);
}

template <class Queue>
void compareWithDeque() {
    Queue values;
    std::deque<int> expected;
    std::mt19937 rng(2026);
    for (int step = 0; step < 3000; ++step) {
        if (expected.empty() || rng() % 3 != 0) {
            const int value = static_cast<int>(rng() % 101) - 50;
            values.push(value);
            expected.push_back(value);
        } else {
            values.pop();
            expected.pop_front();
        }
        assert(values.size() == static_cast<int>(expected.size()));
        if (!expected.empty())
            assert(values.front() == expected.front() && values.back() == expected.back());
    }
}

struct NoDefault {
    int value;
    explicit NoDefault(int x) : value(x) {}
};

void checkArrayWrap() {
    arrayQueue<int> q(1);
    q.push(1);
    assert(q.capacity() == 1 && q.front() == 1);
    q.push(2); // 满队列扩容，顺序仍应为 1,2。
    assert(q.capacity() >= 2 && q.front() == 1 && q.back() == 2);
    q.pop();
    q.push(3); // 尾下标绕回数组开头。
    assert(q.front() == 2 && q.back() == 3);
    q.push(4); // 已环绕且已满时扩容，须按逻辑顺序复制 2,3。
    assert(q.front() == 2 && q.back() == 4 && q.size() == 3);
    arrayQueue<int> copy(q);
    assert(copy.front() == 2 && copy.back() == 4);
    arrayQueue<NoDefault> objects(1);
    objects.push(NoDefault(5));
    objects.push(NoDefault(7));
    objects.pop();
    assert(objects.front().value == 7);
    linkedQueue<NoDefault> linked;
    linked.push(NoDefault(9));
    assert(linked.front().value == 9);
}

bool bruteQueueRailroad(const std::vector<int>& input, std::size_t position,
                        int needed, std::vector<std::deque<int>> tracks) {
    while (true) {
        bool moved = false;
        for (std::deque<int>& track : tracks)
            if (!track.empty() && track.front() == needed) {
                track.pop_front();
                ++needed;
                moved = true;
                break;
            }
        if (!moved) break;
    }
    if (position == input.size()) return needed == static_cast<int>(input.size()) + 1;
    const int car = input[position];
    if (car == needed)
        return bruteQueueRailroad(input, position + 1, needed + 1, tracks);
    for (std::size_t t = 0; t < tracks.size(); ++t) {
        if (!tracks[t].empty() && tracks[t].back() > car) continue;
        std::vector<std::deque<int>> nextTracks = tracks;
        nextTracks[t].push_back(car);
        if (bruteQueueRailroad(input, position + 1, needed, nextTracks)) return true;
    }
    return false;
}

void checkRailroad() {
    assert(rearrangeRailroadWithQueues({3, 4, 2, 1}, 2).success);
    assert(!rearrangeRailroadWithQueues({3, 4, 2, 1}, 1).success);
    assert(rearrangeRailroadWithQueues({1, 2, 3}, 0).success);
    for (int n = 1; n <= 5; ++n) {
        std::vector<int> input;
        for (int i = 1; i <= n; ++i) input.push_back(i);
        do {
            for (int k = 0; k <= 3; ++k) {
                const bool possible = bruteQueueRailroad(input, 0, 1,
                    std::vector<std::deque<int>>(static_cast<std::size_t>(k)));
                const QueueRailroadResult actual = rearrangeRailroadWithQueues(input, k);
                assert(actual.success == possible);
                if (actual.success) {
                    int needed = 1;
                    for (const QueueRailMove& move : actual.moves)
                        if (move.to == "output") assert(move.car == needed++);
                    assert(needed == n + 1);
                }
            }
        } while (std::next_permutation(input.begin(), input.end()));
    }
}

void checkCircuit() {
    const std::vector<std::vector<int>> board = {
        {0, 0, 1}, {1, 0, 1}, {1, 0, 0}
    };
    const std::vector<GridPoint> path = routeCircuit(board, {0, 0}, {2, 2});
    assert(path.size() == 5 && path.front() == GridPoint(0, 0));
    assert(path.back() == GridPoint(2, 2));
    assert(routeCircuit(board, {0, 0}, {1, 2}).empty());

    std::mt19937 rng(9);
    const int dr[4] = {-1, 0, 1, 0};
    const int dc[4] = {0, 1, 0, -1};
    for (int trial = 0; trial < 100; ++trial) {
        std::vector<std::vector<int>> randomBoard(4, std::vector<int>(4, 0));
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                randomBoard[r][c] = static_cast<int>(rng() % 3 == 0);
        randomBoard[0][0] = randomBoard[3][3] = 0;
        int distance[4][4];
        for (auto& row : distance) for (int& x : row) x = -1;
        std::queue<GridPoint> pending;
        pending.push({0, 0});
        distance[0][0] = 0;
        while (!pending.empty()) {
            const GridPoint p = pending.front();
            pending.pop();
            for (int d = 0; d < 4; ++d) {
                const int nr = p.row + dr[d], nc = p.col + dc[d];
                if (nr < 0 || nr >= 4 || nc < 0 || nc >= 4 ||
                    randomBoard[nr][nc] || distance[nr][nc] >= 0) continue;
                distance[nr][nc] = distance[p.row][p.col] + 1;
                pending.push({nr, nc});
            }
        }
        const std::vector<GridPoint> result = routeCircuit(randomBoard, {0, 0}, {3, 3});
        assert((distance[3][3] < 0) == result.empty());
        if (!result.empty()) {
            assert(result.size() == static_cast<std::size_t>(distance[3][3] + 1));
            for (std::size_t i = 1; i < result.size(); ++i)
                assert(std::abs(result[i].row - result[i - 1].row) +
                       std::abs(result[i].col - result[i - 1].col) == 1);
        }
    }
}

void checkComponents() {
    const std::vector<std::vector<int>> image = {
        {1, 0, 1}, {0, 1, 0}, {1, 0, 1}
    };
    const ComponentLabeling four = recognizeComponents(image);
    const ComponentLabeling eight = recognizeComponents(image, true);
    assert(four.count() == 5 && eight.count() == 1);
    for (int size : four.sizes) assert(size == 1);
    assert(eight.sizes[0] == 5);
    assert(recognizeComponents({}).count() == 0);
}

void checkFactory() {
    const std::vector<FactoryJob> jobs = {{1, 0, 3}, {2, 1, 2}, {3, 1, 1}};
    const FactoryResult one = simulateFactory(jobs, 1);
    assert(one.records.size() == 3 && one.makespan == 6);
    assert(one.records[0].id == 1 && one.records[0].start == 0);
    assert(one.records[1].id == 2 && one.records[1].start == 3);
    assert(one.records[2].id == 3 && one.records[2].start == 5);
    assert(std::abs(one.averageWait - 2.0) < 1e-9);
    const FactoryResult two = simulateFactory(jobs, 2);
    assert(two.makespan == 4 && two.records[2].start == 3);
    assert(two.records[2].id == 3 && two.records[2].wait == 2);
    const FactoryResult tie = simulateFactory(
        {{1, 0, 2}, {2, 1, 2}, {3, 2, 1}}, 1);
    assert(tie.records[1].id == 2 && tie.records[1].start == 2);
    assert(tie.records[2].id == 3 && tie.records[2].start == 4);
    assert(simulateFactory({}, 2).records.empty());
    bool threw = false;
    try { simulateFactory({{1, 0, 2}, {1, 1, 3}}, 1); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
}

int main() {
    checkQueue<arrayQueue<int>>();
    checkQueue<linkedQueue<int>>();
    compareWithDeque<arrayQueue<int>>();
    compareWithDeque<linkedQueue<int>>();
    checkArrayWrap();
    checkRailroad();
    checkCircuit();
    checkComponents();
    checkFactory();
}
