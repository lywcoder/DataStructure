#include <cassert>
#include <iostream>

#include "binary.hpp"

// 计数的是循环体执行次数，而不是机器指令数或运行时间。
long long linearVisits(int n) {
    long long visits = 0;
    for (int i = 0; i < n; ++i) ++visits;
    return visits; // n
}

long long triangularPairs(int n) {
    long long pairs = 0;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            ++pairs;
    return pairs; // 0 + 1 + ... + (n - 1)
}

int halvingSteps(unsigned int n) {
    int steps = 0;
    while (n > 1) {
        n /= 2;
        ++steps;
    }
    return steps; // n >= 1 时为 floor(log2(n))
}

long long workAtEveryLevel(int n) {
    long long work = 0;
    for (int remaining = n; remaining > 1; remaining /= 2)
        for (int i = 0; i < n; ++i)
            ++work;
    return work; // n * floor(log2(n))，n >= 1
}

struct CountedInt {
    int value;
    static int equalityChecks;
    static int greaterChecks;

    explicit CountedInt(int v) : value(v) {}
};

int CountedInt::equalityChecks = 0;
int CountedInt::greaterChecks = 0;

bool operator==(const CountedInt& a, const CountedInt& b) {
    ++CountedInt::equalityChecks;
    return a.value == b.value;
}

bool operator>(const CountedInt& a, const CountedInt& b) {
    ++CountedInt::greaterChecks;
    return a.value > b.value;
}

int main() {
    std::cout << "n  linear  pairs  halving  n*halving\n";
    for (int n = 1; n <= 32; n *= 2) {
        const int levels = halvingSteps(static_cast<unsigned int>(n));
        assert(linearVisits(n) == n);
        assert(triangularPairs(n) == static_cast<long long>(n) * (n - 1) / 2);
        assert(workAtEveryLevel(n) == static_cast<long long>(n) * levels);
        std::cout << n << "  " << linearVisits(n) << "  " << triangularPairs(n)
                  << "  " << levels << "  " << workAtEveryLevel(n) << '\n';
    }

    const CountedInt sorted[] = {CountedInt(0), CountedInt(2), CountedInt(4),
                                 CountedInt(6), CountedInt(8), CountedInt(10),
                                 CountedInt(12), CountedInt(14)};

    CountedInt::equalityChecks = CountedInt::greaterChecks = 0;
    const int found = binarySearch(sorted, 8, CountedInt(6));
    assert(found == 3);
    assert(CountedInt::equalityChecks == 1); // 首次取中点就找到

    CountedInt::equalityChecks = CountedInt::greaterChecks = 0;
    const int missing = binarySearch(sorted, 8, CountedInt(15));
    assert(missing == -1);
    assert(CountedInt::equalityChecks == 4); // 区间长度 8 -> 4 -> 2 -> 1 -> 0
    assert(CountedInt::greaterChecks == 4);
    std::cout << "binarySearch: n=8, missing=15, midpoint checks="
              << CountedInt::equalityChecks << '\n';

    assert(binarySearch(sorted, 0, CountedInt(1)) == -1);
    assert(binarySearch(static_cast<const CountedInt*>(nullptr), 0,
                        CountedInt(1)) == -1);

    const int repeated[] = {1, 2, 2, 2, 3};
    const int duplicateIndex = binarySearch(repeated, 5, 2);
    assert(duplicateIndex >= 1 && duplicateIndex <= 3);
    assert(binarySearch(repeated, 5, 0) == -1);
    assert(binarySearch(repeated, 5, 4) == -1);
}
