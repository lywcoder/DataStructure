#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
// 纯教学模型：一个缓存行装 4 个 int，缓存最多放 2 行，按 LRU 淘汰。
struct Counts {
    int hits;
    int misses;
};

Counts simulateFourByFour(bool rowFirst) {
    std::deque<std::size_t> cachedLines; // 队首是最近使用的行。
    Counts counts = {0, 0};
    for (std::size_t outer = 0; outer < 4; ++outer) {
        for (std::size_t inner = 0; inner < 4; ++inner) {
            const std::size_t row = rowFirst ? outer : inner;
            const std::size_t col = rowFirst ? inner : outer;
            const std::size_t line = (row * 4 + col) / 4;
            std::deque<std::size_t>::iterator found =
                std::find(cachedLines.begin(), cachedLines.end(), line);
            if (found == cachedLines.end()) {
                ++counts.misses;
                if (cachedLines.size() == 2) cachedLines.pop_back();
            } else {
                ++counts.hits;
                cachedLines.erase(found);
            }
            cachedLines.push_front(line);
        }
    }
    return counts;
}

std::uint64_t sumRows(const std::vector<int>& data, std::size_t side) {
    std::uint64_t total = 0;
    for (std::size_t row = 0; row < side; ++row)
        for (std::size_t col = 0; col < side; ++col)
            total += static_cast<unsigned int>(data[row * side + col]);
    return total;
}

std::uint64_t sumColumns(const std::vector<int>& data, std::size_t side) {
    std::uint64_t total = 0;
    for (std::size_t col = 0; col < side; ++col)
        for (std::size_t row = 0; row < side; ++row)
            total += static_cast<unsigned int>(data[row * side + col]);
    return total;
}

typedef std::uint64_t (*Traversal)(const std::vector<int>&, std::size_t);

double measureMilliseconds(const std::vector<int>& data, std::size_t side,
                           Traversal traverse, std::uint64_t expected) {
    typedef std::chrono::steady_clock Clock;
    const Clock::time_point start = Clock::now();
    const std::uint64_t sum = traverse(data, side);
    const Clock::time_point stop = Clock::now();
    if (sum != expected) throw std::runtime_error("different matrix sums");
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

double median(std::vector<double>& samples) {
    std::sort(samples.begin(), samples.end());
    return samples[samples.size() / 2];
}
} // namespace

int main() {
    const Counts rows = simulateFourByFour(true);
    const Counts columns = simulateFourByFour(false);
    if (rows.hits != 12 || rows.misses != 4 ||
        columns.hits != 0 || columns.misses != 16)
        throw std::runtime_error("cache model counts are incorrect");
    std::cout << "toy 4x4 / line=4 integers / capacity=2 lines:\n";
    std::cout << "row first:    " << rows.hits << " hits, " << rows.misses << " misses\n";
    std::cout << "column first: " << columns.hits << " hits, " << columns.misses << " misses\n";

    const std::size_t side = 2048; // 约 16 MiB 的行主序数组。
    std::vector<int> data(side * side);
    std::uint64_t expected = 0;
    for (std::size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<int>(i % 251);
        expected += static_cast<unsigned int>(data[i]);
    }

    std::vector<double> rowSamples;
    std::vector<double> columnSamples;
    for (int repetition = 0; repetition < 9; ++repetition) {
        // 交替顺序，减轻“总是先测某一种”带来的偏差；前两轮热身。
        if (repetition % 2 == 0) {
            const double row = measureMilliseconds(data, side, sumRows, expected);
            const double column = measureMilliseconds(data, side, sumColumns, expected);
            if (repetition >= 2) {
                rowSamples.push_back(row);
                columnSamples.push_back(column);
            }
        } else {
            const double column = measureMilliseconds(data, side, sumColumns, expected);
            const double row = measureMilliseconds(data, side, sumRows, expected);
            if (repetition >= 2) {
                rowSamples.push_back(row);
                columnSamples.push_back(column);
            }
        }
    }

    std::cout << "2048x2048 matrix, median milliseconds (7 samples):\n";
    std::cout << std::fixed << std::setprecision(2)
              << "row first: " << median(rowSamples) << '\n'
              << "column first: " << median(columnSamples) << '\n';
}
