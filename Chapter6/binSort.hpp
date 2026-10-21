#include <iostream>
#include "chain.hpp"

struct studentRecord {
    int score;
    std::string* name;
    int operator!=(const studentRecord& x) const {
        return (score != x.score);
    }

    // int operator!=(const studentRecord& x) const {
    //     return (score != x.score);
    // }
    operator int() const { return score; }
    //从studentRecord到int的类型转换
};

std::ostream& operator<<(std::ostream& out, const studentRecord& x); 

void binSort(chain<studentRecord>& theChain, int range);