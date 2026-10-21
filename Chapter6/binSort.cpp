#include "binSort.hpp"

std::ostream& operator<<(std::ostream& out, const studentRecord& record) {
    return out << '(' << record.score << ", " << record.name << ')';
}

// chain::binSort 按分数稳定地重接结点：同分记录保持原来的先后次序。
void binSort(chain<studentRecord>& records, int maxScore) {
    records.binSort(maxScore);
}
