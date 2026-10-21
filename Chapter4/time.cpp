#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

#include "insert.hpp"

namespace {
volatile int observedValue = 0;

std::vector<int> makeInput(int n, int kind) {
    std::vector<int> input(static_cast<std::size_t>(n));
    std::iota(input.begin(), input.end(), 0);
    if (kind == 1) {
        std::reverse(input.begin(), input.end());
    } else if (kind == 2) {
        std::mt19937 rng(static_cast<unsigned int>(2026 + n));
        std::shuffle(input.begin(), input.end(), rng);
    }
    return input;
}

double medianSortMicroseconds(const std::vector<int>& input) {
    typedef std::chrono::steady_clock Clock;
    std::vector<double> measurements;
    const int n = static_cast<int>(input.size());

    // 每轮在计时前恢复同一份输入；先运行几轮热身。
    for (int repetition = 0; repetition < 28; ++repetition) {
        std::vector<int> work = input;
        const Clock::time_point start = Clock::now();
        insertionSort(work.data(), n);
        const Clock::time_point stop = Clock::now();

        for (int i = 0; i < n; ++i)
            if (work[static_cast<std::size_t>(i)] != i)
                throw std::runtime_error("insertionSort returned incorrect data");
        observedValue ^= work[static_cast<std::size_t>(n / 2)];

        if (repetition >= 3) {
            const double microseconds =
                std::chrono::duration<double, std::micro>(stop - start).count();
            measurements.push_back(microseconds);
        }
    }

    std::sort(measurements.begin(), measurements.end());
    return measurements[measurements.size() / 2];
}
} // namespace

int main() {
    std::cout << "insertion sort: median microseconds (25 samples)\n";
    std::cout << "n        sorted    reversed    shuffled\n";
    for (int n = 256; n <= 2048; n *= 2) {
        std::cout << std::setw(4) << n;
        for (int kind = 0; kind < 3; ++kind)
            std::cout << std::setw(12)
                      << std::fixed << std::setprecision(2)
                      << medianSortMicroseconds(makeInput(n, kind));
        std::cout << '\n';
    }
    return 0;
}
