#ifndef CHAPTER8_HANOI_HPP
#define CHAPTER8_HANOI_HPP

#include "arraystack.hpp"
#include <array>
#include <ostream>
#include <stdexcept>
#include <vector>

struct HanoiMove {
    int disk;
    int from;
    int to;
};

struct HanoiFrame {
    int count;
    int from;
    int to;
    int auxiliary;
    bool transfer; // true 表示执行一次真实搬移；false 表示展开子问题。
    HanoiFrame(int n, int source, int target, int spare, bool move)
        : count(n), from(source), to(target), auxiliary(spare), transfer(move) {}
};

// 用“任务栈”显式模拟递归，同时用三个磁盘栈检查每步是否合法。
inline std::vector<HanoiMove> solveHanoi(int diskCount) {
    if (diskCount < 0 || diskCount > 20)
        throw std::invalid_argument("diskCount must be in [0,20]");
    std::array<arrayStack<int>, 4> towers; // 使用下标 1、2、3。
    for (int disk = diskCount; disk >= 1; --disk) towers[1].push(disk);
    arrayStack<HanoiFrame> tasks;
    tasks.push(HanoiFrame(diskCount, 1, 3, 2, false));
    std::vector<HanoiMove> moves;
    while (!tasks.empty()) {
        const HanoiFrame task = tasks.top();
        tasks.pop();
        if (task.transfer) {
            if (towers[task.from].empty()) throw std::logic_error("empty source tower");
            const int disk = towers[task.from].top();
            if (!towers[task.to].empty() && towers[task.to].top() < disk)
                throw std::logic_error("larger disk on smaller disk");
            towers[task.from].pop();
            towers[task.to].push(disk);
            moves.push_back(HanoiMove{disk, task.from, task.to});
        } else if (task.count > 0) {
            // 栈后进先出，所以按“右子问题、搬最大盘、左子问题”逆序压入。
            tasks.push(HanoiFrame(task.count - 1, task.auxiliary, task.to,
                                  task.from, false));
            tasks.push(HanoiFrame(0, task.from, task.to, task.auxiliary, true));
            tasks.push(HanoiFrame(task.count - 1, task.from, task.auxiliary,
                                  task.to, false));
        }
    }
    if (towers[3].size() != diskCount)
        throw std::logic_error("Hanoi solution did not finish");
    return moves;
}

inline void printHanoiMoves(int diskCount, std::ostream& out) {
    for (const HanoiMove& move : solveHanoi(diskCount))
        out << "disk " << move.disk << ": " << move.from << " -> " << move.to << '\n';
}

#endif
