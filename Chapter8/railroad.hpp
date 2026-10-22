#ifndef CHAPTER8_RAILROAD_HPP
#define CHAPTER8_RAILROAD_HPP

#include "arraystack.hpp"
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

struct RailMove {
    int car;
    std::string from;
    std::string to;
};
struct RailroadResult {
    bool success;
    int nextNeeded; // 未成功时尚未输出的最小编号。
    std::vector<RailMove> moves;
};

// 输入车厢编号必须是 1..n 的一个排列。缓冲轨道各是一只栈，
// 轨道内从栈底到栈顶保持严格递减，才能按编号递增弹出。
inline RailroadResult rearrangeRailroad(const std::vector<int>& input,
                                        int holdingTrackCount) {
    if (holdingTrackCount < 0)
        throw std::invalid_argument("negative holding track count");
    if (holdingTrackCount == std::numeric_limits<int>::max() ||
        input.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::length_error("too many cars");
    const int n = static_cast<int>(input.size());
    std::vector<char> seen(input.size() + 1, 0);
    for (int car : input) {
        if (car < 1 || car > n || seen[static_cast<std::size_t>(car)])
            throw std::invalid_argument("input must be a permutation of 1..n");
        seen[static_cast<std::size_t>(car)] = 1;
    }

    std::vector<arrayStack<int>> tracks(static_cast<std::size_t>(holdingTrackCount + 1));
    RailroadResult result = {false, 1, {}};
    int next = 1;
    const auto drain = [&]() {
        while (true) {
            int ready = 0;
            for (int t = 1; t <= holdingTrackCount; ++t)
                if (!tracks[static_cast<std::size_t>(t)].empty() &&
                    tracks[static_cast<std::size_t>(t)].top() == next) {
                    ready = t;
                    break;
                }
            if (ready == 0) break;
            tracks[static_cast<std::size_t>(ready)].pop();
            result.moves.push_back(RailMove{next, "holding " + std::to_string(ready), "output"});
            ++next;
        }
    };

    for (int car : input) {
        if (car == next) {
            result.moves.push_back(RailMove{car, "input", "output"});
            ++next;
            drain();
            continue;
        }

        int best = 0, firstEmpty = 0, bestTop = n + 1;
        for (int t = 1; t <= holdingTrackCount; ++t) {
            const arrayStack<int>& track = tracks[static_cast<std::size_t>(t)];
            if (track.empty()) {
                if (firstEmpty == 0) firstEmpty = t;
            } else if (car < track.top() && track.top() < bestTop) {
                best = t;
                bestTop = track.top();
            }
        }
        if (best == 0) best = firstEmpty;
        if (best == 0) {
            result.nextNeeded = next;
            return result;
        }
        tracks[static_cast<std::size_t>(best)].push(car);
        result.moves.push_back(RailMove{car, "input", "holding " + std::to_string(best)});
    }
    drain();
    result.nextNeeded = next;
    result.success = next == n + 1;
    return result;
}

inline void printRailroadMoves(const RailroadResult& result, std::ostream& out) {
    for (const RailMove& move : result.moves)
        out << "car " << move.car << ": " << move.from << " -> " << move.to << '\n';
    out << (result.success ? "rearrangement succeeded" : "rearrangement failed") << '\n';
}

#endif
