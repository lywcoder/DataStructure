#ifndef CHAPTER5_LINEAR_LIST_HPP
#define CHAPTER5_LINEAR_LIST_HPP

#include <ostream>

// 线性表的抽象接口：只规定操作，不规定数组或链表等存储方式。
template <class T>
class linearList {
public:
    virtual ~linearList() {}
    virtual bool empty() const = 0;
    virtual int size() const = 0;
    // 保留教材接口以兼容后续章节；const 对象仍能得到可写引用，见 README。
    virtual T& get(int theIndex) const = 0;
    virtual int indexOf(const T& theElement) const = 0;
    virtual void erase(int theIndex) = 0;
    virtual void insert(int theIndex, const T& theElement) = 0;
    virtual void output(std::ostream& out) const = 0;
};

#endif
