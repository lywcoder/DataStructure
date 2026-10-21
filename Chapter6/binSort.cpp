#include "binSort.hpp"
#include <ostream>

void binSort(chain<studentRecord> &theChain, int range) {
    //对箱子进行初始化
    chain<studentRecord> *bin;
    bin = new chain<studentRecord> [range + 1];
    
    //把学生记录从链表取出，然后分配到箱子里
    int numberOfElements = theChain.size();
    for (int i = 1; i < numberOfElements; i++) {
        studentRecord x = theChain.get(0);
        theChain.erase(0);
        bin[x.score].insert(0, x);
    }
    
    //从箱子中收集元素
    for (int j = range; j >= 0; j--) {
        while (!bin[j].empty()) {
            studentRecord x= bin[j].get(0);
            bin[j].erase(0);
            theChain.insert(0, x);
        }
    }
    
    delete[] bin;

}

std::ostream& operator<<(std::ostream& out, const studentRecord& x) {
    out << x.score << ' ' << *x.name << std::endl;
    return out;
}
