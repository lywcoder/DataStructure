#ifndef CHAPTER6_EXTENDED_LINEAR_LIST_HPP
#define CHAPTER6_EXTENDED_LINEAR_LIST_HPP

#include "../Chapter5/linearList.hpp"

// 在基本线性表操作上增加清空和尾插。
template <class T>
class extendedLinearList : public linearList<T> {
public:
    ~extendedLinearList() override {}
    virtual void clear() = 0;
    virtual void push_back(const T& value) = 0;
};

#endif
