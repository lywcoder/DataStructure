#include "bracket.hpp"
#include "hanoi.hpp"
#include "linkedstack.hpp"
#include "maze.hpp"
#include "offlineEquivalence.hpp"
#include "railroad.hpp"
#include "switchBox.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <queue>
#include <random>
#include <stdexcept>
#include <vector>

template <class Stack>
void checkStack() {
    Stack values(1);
    assert(values.empty() && values.size() == 0);
    bool threw = false;
    try { values.pop(); } catch (const stackEmpty&) { threw = true; }
    assert(threw);
    values.push(10);
    values.push(values.top()); // 从自身栈顶再次压入，扩容也应安全。
    values.push(30);
    assert(values.size() == 3 && values.top() == 30);
    Stack copy(values);
    copy.pop();
    assert(copy.top() == 10 && values.top() == 30);
    Stack assigned;
    assigned = values;
    assigned = assigned;
    assert(assigned.top() == 30);
    const Stack& constView = values;
    assert(constView.top() == 30);
    stack<int>& base = values;
    base.pop();
    assert(base.size() == 2 && base.top() == 10);
}

void checkBrackets() {
    const BracketMatchResult good = matchBrackets("a{b[c](d)}");
    assert(good.matched() && good.pairs.size() == 3);
    const BracketMatchResult bad = matchBrackets("([)]");
    assert(!bad.matched());
    assert(bad.unmatchedLeft.size() == 1 && bad.unmatchedLeft[0] == 0);
    assert(bad.unmatchedRight.size() == 1 && bad.unmatchedRight[0] == 2);
    assert(bad.pairs.size() == 1 && bad.pairs[0].first == 1 && bad.pairs[0].second == 3);
    assert(matchBrackets("text only").matched());
}

void checkHanoi() {
    assert(solveHanoi(0).empty());
    for (int n = 1; n <= 8; ++n) {
        const std::vector<HanoiMove> moves = solveHanoi(n);
        assert(moves.size() == (static_cast<std::size_t>(1) << n) - 1);
        std::vector<int> towers[4];
        for (int disk = n; disk >= 1; --disk) towers[1].push_back(disk);
        for (const HanoiMove& move : moves) {
            assert(!towers[move.from].empty() && towers[move.from].back() == move.disk);
            towers[move.from].pop_back();
            assert(towers[move.to].empty() || towers[move.to].back() > move.disk);
            towers[move.to].push_back(move.disk);
        }
        assert(towers[3].size() == static_cast<std::size_t>(n));
    }
}

// 穷举所有合法缓冲轨道选择，作为列车贪心算法的独立参照。
bool bruteRailroad(const std::vector<int>& input, std::size_t position, int needed,
                   std::vector<std::vector<int>> tracks) {
    while (true) {
        bool moved = false;
        for (std::vector<int>& track : tracks)
            if (!track.empty() && track.back() == needed) {
                track.pop_back();
                ++needed;
                moved = true;
                break;
            }
        if (!moved) break;
    }
    if (position == input.size()) return needed == static_cast<int>(input.size()) + 1;
    const int car = input[position];
    if (car == needed) return bruteRailroad(input, position + 1, needed + 1, tracks);
    for (std::size_t t = 0; t < tracks.size(); ++t) {
        if (!tracks[t].empty() && tracks[t].back() < car) continue;
        std::vector<std::vector<int>> nextTracks = tracks;
        nextTracks[t].push_back(car);
        if (bruteRailroad(input, position + 1, needed, nextTracks)) return true;
    }
    return false;
}

void checkRailroad() {
    assert(rearrangeRailroad({3, 4, 2, 1}, 2).success);
    assert(!rearrangeRailroad({3, 4, 2, 1}, 1).success);
    assert(rearrangeRailroad({1, 2, 3}, 0).success);
    bool threw = false;
    try { rearrangeRailroad({1, 1}, 2); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    for (int n = 1; n <= 5; ++n) {
        std::vector<int> input;
        for (int i = 1; i <= n; ++i) input.push_back(i);
        do {
            for (int k = 0; k <= 3; ++k) {
                std::vector<std::vector<int>> tracks(static_cast<std::size_t>(k));
                const bool possible = bruteRailroad(input, 0, 1, tracks);
                const RailroadResult actual = rearrangeRailroad(input, k);
                assert(actual.success == possible);
                if (actual.success) {
                    int next = 1;
                    for (const RailMove& move : actual.moves)
                        if (move.to == "output") assert(move.car == next++);
                    assert(next == n + 1);
                }
            }
        } while (std::next_permutation(input.begin(), input.end()));
    }
}

void checkOtherApplications() {
    assert(switchBoxRoutable({1, 2, 2, 1, 3, 3}));
    assert(!switchBoxRoutable({1, 2, 1, 2}));
    bool threw = false;
    try { switchBoxRoutable({1, 2, 1}); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    const std::vector<std::vector<int>> classes = offlineEquivalenceClasses(
        6, {{0, 1}, {1, 2}, {3, 4}});
    assert((classes == std::vector<std::vector<int>>{{0, 1, 2}, {3, 4}, {5}}));

    const std::vector<std::vector<int>> grid = {
        {0, 0, 1}, {1, 0, 1}, {1, 0, 0}
    };
    const std::vector<std::pair<int, int>> route = findMazePath(grid, {0, 0}, {2, 2});
    assert(route.front() == std::make_pair(0, 0));
    assert(route.back() == std::make_pair(2, 2));
    for (std::size_t i = 0; i < route.size(); ++i) {
        assert(grid[static_cast<std::size_t>(route[i].first)]
                   [static_cast<std::size_t>(route[i].second)] == 0);
        if (i != 0)
            assert(std::abs(route[i].first - route[i - 1].first) +
                   std::abs(route[i].second - route[i - 1].second) == 1);
    }
    assert(findMazePath(grid, {0, 0}, {1, 2}).empty()); // 终点是墙。

    // 小迷宫随机对照：DFS 是否找到路径应与 BFS 可达性一致。
    std::mt19937 rng(8);
    for (int trial = 0; trial < 100; ++trial) {
        std::vector<std::vector<int>> randomGrid(4, std::vector<int>(4, 0));
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                randomGrid[r][c] = static_cast<int>(rng() % 3 == 0);
        randomGrid[0][0] = randomGrid[3][3] = 0;
        std::queue<std::pair<int, int>> pending;
        bool visited[4][4] = {};
        pending.push({0, 0});
        visited[0][0] = true;
        const int dr[4] = {-1, 0, 1, 0};
        const int dc[4] = {0, 1, 0, -1};
        while (!pending.empty()) {
            const std::pair<int, int> current = pending.front();
            pending.pop();
            for (int d = 0; d < 4; ++d) {
                const int nr = current.first + dr[d], nc = current.second + dc[d];
                if (nr < 0 || nr >= 4 || nc < 0 || nc >= 4 ||
                    randomGrid[nr][nc] || visited[nr][nc]) continue;
                visited[nr][nc] = true;
                pending.push({nr, nc});
            }
        }
        const bool dfsFound = !findMazePath(randomGrid, {0, 0}, {3, 3}).empty();
        assert(dfsFound == visited[3][3]);
    }
}

int main() {
    checkStack<arrayStack<int>>();
    checkStack<linkedStack<int>>();
    checkBrackets();
    checkHanoi();
    checkRailroad();
    checkOtherApplications();
}
