#ifndef linearList_
#define linearList_

#include "../Chapter5/linearList.hpp"

// 扩展的线性表抽象基类，在 linearList 基础上增加 clear 和 push_back
template <class T>
class extendedLinearList : linearList<T> {
public:
    virtual ~extendedLinearList() {}
    virtual void clear() = 0;
    virtual void push_back(const T& theElement) = 0;
};

#endif

