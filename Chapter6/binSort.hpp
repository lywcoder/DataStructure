#ifndef CHAPTER6_BIN_SORT_HPP
#define CHAPTER6_BIN_SORT_HPP

#include "chain.hpp"
#include <ostream>
#include <string>

struct studentRecord {
    int score;
    std::string name; // 值语义，避免管理裸字符串指针的生命期。

    studentRecord() : score(0), name() {}
    studentRecord(int s, const std::string& n) : score(s), name(n) {}
    bool operator==(const studentRecord& other) const {
        return score == other.score && name == other.name;
    }
    operator int() const { return score; } // chain::binSort 的桶号。
};

std::ostream& operator<<(std::ostream& out, const studentRecord& record);
void binSort(chain<studentRecord>& records, int maxScore);

#endif
