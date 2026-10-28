#ifndef CHAPTER12_MACHINE_SCHEDULING_HPP
#define CHAPTER12_MACHINE_SCHEDULING_HPP

#include <algorithm>
#include <cstdint>
#include <limits>
#include <queue>
#include <set>
#include <stdexcept>
#include <vector>

struct schedulingJob {
    int id;
    std::int64_t duration;
    schedulingJob(int jobId, std::int64_t time) : id(jobId), duration(time) {}
};

struct schedulingAssignment {
    int jobId;
    int machineId;
    std::int64_t start;
    std::int64_t finish;
};

struct schedulingResult {
    std::vector<schedulingAssignment> assignments;
    std::int64_t makespan = 0;
};

// LPT：长作业优先；空闲时间最早的机器优先。机器编号为 0..machineCount-1。
// 这是启发式调度，保证不重叠，但不保证最短完工时间。
inline schedulingResult scheduleJobs(std::vector<schedulingJob> jobs, int machineCount) {
    if (machineCount <= 0) throw std::invalid_argument("machine count must be positive");
    std::set<int> ids;
    for (const schedulingJob& job : jobs) {
        if (job.duration < 0) throw std::invalid_argument("negative job duration");
        if (!ids.insert(job.id).second) throw std::invalid_argument("duplicate job id");
    }
    std::sort(jobs.begin(), jobs.end(), [](const schedulingJob& a, const schedulingJob& b) {
        if (a.duration != b.duration) return a.duration > b.duration;
        return a.id < b.id;
    });
    struct Machine {
        std::int64_t available;
        int id;
    };
    struct Earlier {
        bool operator()(const Machine& a, const Machine& b) const {
            if (a.available != b.available) return a.available > b.available;
            return a.id > b.id;
        }
    };
    std::priority_queue<Machine, std::vector<Machine>, Earlier> machines;
    for (int i = 0; i < machineCount; ++i) machines.push(Machine{0, i});
    schedulingResult result;
    for (const schedulingJob& job : jobs) {
        Machine machine = machines.top(); machines.pop();
        if (job.duration > std::numeric_limits<std::int64_t>::max() - machine.available)
            throw std::overflow_error("schedule time overflow");
        const std::int64_t finish = machine.available + job.duration;
        result.assignments.push_back(schedulingAssignment{job.id, machine.id,
                                                            machine.available, finish});
        machine.available = finish;
        machines.push(machine);
        if (finish > result.makespan) result.makespan = finish;
    }
    return result;
}

#endif
