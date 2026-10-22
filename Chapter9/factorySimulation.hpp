#ifndef CHAPTER9_FACTORY_SIMULATION_HPP
#define CHAPTER9_FACTORY_SIMULATION_HPP

#include "linkedQueue.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <unordered_set>
#include <vector>

struct FactoryJob {
    int id;
    long long arrival;
    long long duration;
};
struct FactoryRecord {
    int id;
    int machine; // 机器编号从 1 开始。
    long long arrival;
    long long start;
    long long finish;
    long long wait;
};
struct FactoryResult {
    std::vector<FactoryRecord> records; // 按开始加工的顺序排列。
    long long makespan;
    double averageWait;
    double utilization; // 所有机器忙碌时间 / (机器数 * 完成时刻)。
};

// 多台相同机器共用一条 FIFO 等待队列。
// 时钟只跳到下一次到达或完成事件，避免逐个时间单位空转。
inline FactoryResult simulateFactory(std::vector<FactoryJob> jobs, int machineCount) {
    if (machineCount < 1)
        throw std::invalid_argument("machineCount must be positive");
    std::unordered_set<int> ids;
    for (const FactoryJob& job : jobs) {
        if (job.arrival < 0 || job.arrival == std::numeric_limits<long long>::max() ||
            job.duration <= 0 || !ids.insert(job.id).second)
            throw std::invalid_argument("invalid arrival, duration, or duplicate id");
    }
    std::stable_sort(jobs.begin(), jobs.end(),
        [](const FactoryJob& a, const FactoryJob& b) { return a.arrival < b.arrival; });

    const long long infinity = std::numeric_limits<long long>::max();
    std::vector<long long> busyUntil(static_cast<std::size_t>(machineCount), -1);
    linkedQueue<FactoryJob> waiting;
    FactoryResult result = {{}, 0, 0.0, 0.0};
    std::size_t nextJob = 0;
    long double totalWait = 0, totalBusy = 0;
    while (true) {
        const long long nextArrival = nextJob < jobs.size() ? jobs[nextJob].arrival : infinity;
        long long nextFinish = infinity;
        for (long long finish : busyUntil)
            if (finish >= 0 && finish < nextFinish) nextFinish = finish;
        if (nextArrival == infinity && nextFinish == infinity) break;
        const long long now = std::min(nextArrival, nextFinish);

        for (long long& finish : busyUntil)
            if (finish == now) finish = -1; // 先释放本时刻完成的机器。
        while (nextJob < jobs.size() && jobs[nextJob].arrival == now)
            waiting.push(jobs[nextJob++]); // 同时到达时保留输入相对顺序。

        for (int m = 0; m < machineCount && !waiting.empty(); ++m) {
            if (busyUntil[static_cast<std::size_t>(m)] >= 0) continue;
            const FactoryJob job = waiting.front();
            waiting.pop();
            if (job.duration >= infinity - now)
                throw std::overflow_error("factory finish time overflow");
            const long long finish = now + job.duration;
            busyUntil[static_cast<std::size_t>(m)] = finish;
            result.records.push_back(FactoryRecord{
                job.id, m + 1, job.arrival, now, finish, now - job.arrival});
            totalWait += static_cast<long double>(now - job.arrival);
            totalBusy += static_cast<long double>(job.duration);
            if (finish > result.makespan) result.makespan = finish;
        }
    }
    if (!jobs.empty())
        result.averageWait = static_cast<double>(totalWait / jobs.size());
    if (result.makespan > 0)
        result.utilization = static_cast<double>(
            totalBusy / (static_cast<long double>(machineCount) * result.makespan));
    return result;
}

#endif
