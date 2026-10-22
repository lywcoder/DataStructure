#ifndef CHAPTER9_RAILROAD_HPP
#define CHAPTER9_RAILROAD_HPP

#include "arrayQueue.hpp"
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

struct QueueRailMove {
    int car;
    std::string from;
    std::string to;
};
struct QueueRailroadResult {
    bool success;
    int nextNeeded;
    std::vector<QueueRailMove> moves; // 失败时也保留已完成的搬移。
};

// 每条缓冲轨道是 FIFO 队列：从队首到队尾保持严格递增。
// 放入 c 时选 back<c 且 back 最大的非空轨道；否则选第一条空轨道。
inline QueueRailroadResult rearrangeRailroadWithQueues(
    const std::vector<int>& input, int holdingTrackCount) {
    if (holdingTrackCount < 0)
        throw std::invalid_argument("negative holding track count");
    if (holdingTrackCount == std::numeric_limits<int>::max() ||
        input.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::length_error("too many cars or tracks");
    const int n = static_cast<int>(input.size());
    std::vector<char> seen(input.size() + 1, 0);
    for (int car : input) {
        if (car < 1 || car > n || seen[static_cast<std::size_t>(car)])
            throw std::invalid_argument("input must be a permutation of 1..n");
        seen[static_cast<std::size_t>(car)] = 1;
    }

    std::vector<arrayQueue<int>> tracks(static_cast<std::size_t>(holdingTrackCount + 1));
    QueueRailroadResult result = {false, 1, {}};
    int next = 1;
    const auto drain = [&]() {
        while (true) {
            int ready = 0;
            for (int t = 1; t <= holdingTrackCount; ++t)
                if (!tracks[static_cast<std::size_t>(t)].empty() &&
                    tracks[static_cast<std::size_t>(t)].front() == next) {
                    ready = t;
                    break;
                }
            if (ready == 0) break;
            tracks[static_cast<std::size_t>(ready)].pop();
            result.moves.push_back(QueueRailMove{
                next, "holding " + std::to_string(ready), "output"});
            ++next;
        }
    };

    for (int car : input) {
        if (car == next) {
            result.moves.push_back(QueueRailMove{car, "input", "output"});
            ++next;
            drain();
            continue;
        }
        int best = 0, firstEmpty = 0, bestBack = 0;
        for (int t = 1; t <= holdingTrackCount; ++t) {
            const arrayQueue<int>& track = tracks[static_cast<std::size_t>(t)];
            if (track.empty()) {
                if (firstEmpty == 0) firstEmpty = t;
            } else if (track.back() < car && track.back() > bestBack) {
                best = t;
                bestBack = track.back();
            }
        }
        if (best == 0) best = firstEmpty;
        if (best == 0) {
            result.nextNeeded = next;
            return result;
        }
        tracks[static_cast<std::size_t>(best)].push(car);
        result.moves.push_back(QueueRailMove{
            car, "input", "holding " + std::to_string(best)});
    }
    drain();
    result.nextNeeded = next;
    result.success = next == n + 1;
    return result;
}

inline void printQueueRailroadMoves(const QueueRailroadResult& result, std::ostream& out) {
    for (const QueueRailMove& move : result.moves)
        out << "car " << move.car << ": " << move.from << " -> " << move.to << '\n';
    out << (result.success ? "rearrangement succeeded" : "rearrangement failed") << '\n';
}

#endif
