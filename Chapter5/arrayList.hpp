#include "linearList.hpp"
#include <algorithm>
#include <iterator>
#include <ostream>
#include <sstream>
#include <iostream>

template<class T>
class arrayList : public linearList<T> {
public:
    //构造函数，复制构造函数和析构函数
    arrayList(int initialCapacity = 10) ;
    arrayList(const arrayList<T>&);
    ~arrayList() { delete [] element; }

    //ADT方法
    bool empty() const { return listSize == 0; }
    int size() const { return listSize; }
    T& get(int theIndex) const;
    int indexOf(const T& theElement) const;
    void erase(int theIndex);
    void insert(int theIndex, const T& theElement);
    void output(std::ostream& out) const;

    //其他方法
    int capacity() const { return arrayLength; }
    
protected:
    void checkIndex(int theIndex) const;
    //如果索引theIndex无效，就抛出异常
    T* element; //存储线性表元素的一维数组
    int arrayLength; //一维数组的容量
    int listSize; //线性表的元素个数
};

template<class T>
arrayList<T>::arrayList(int initialCapacity) {
    //构造函数
    if (initialCapacity < 1) {
        std::ostringstream s;
        s << "Initial capacity = " << initialCapacity << " Must be > 0";
        throw illegalParameterValue(s.str());
    }
    arrayLength = initialCapacity;
    element = new T[arrayLength];
    listSize = 0;
}

template<class T>
arrayList<T>::arrayList(const arrayList<T>& theList) {
    //复制构造函数
    arrayLength = theList.arrayLength;
    listSize = theList.listSize;
    element = new T[arrayLength];
    copy(theList.element, theList.element + + listSize, element);
}

template<class T>
void arrayList<T>::checkIndex(int theIndex) const {
    //确定索引在0和listSize - 1之间
    if (theIndex < 0 || theIndex >= listSize) {
        std::ostringstream s;
        s << "index = " << theIndex << " size = " << listSize;
        throw illegalIndex(s.str());
    }
}

template<class T>
T& arrayList<T>::get(int theIndex) const {
    //返回索引为theIndex的元素
    checkIndex(theIndex);
    return element[theIndex];
}

template<class T>
int arrayList<T>::indexOf(const T& theElement) const {
    //返回元素theElement第一次出现时候的索引
    //如果该元素不存在，就返回-1
    int theIndex = (int)(find(element, element + listSize, theElement) - element);

    //确定元素theElement是否找到
    if (theIndex == listSize) {
        return -1;
    } else return theIndex;
}

template<class T>
void arrayList<T>::erase(int theIndex) {
    checkIndex(theIndex);

    copy(element + theIndex + 1, element + listSize, element + theIndex);

    element[--listSize].~T();
}

template<class T>
void arrayList<T>::insert(int theIndex, const T& theElement) {
    if (theIndex < 0 || theIndex > listSize) {
        std::ostringstream s;
        s << "index = " << theIndex << " size = " << listSize;
        throw illegalIndex(s.str());
    }

    if (listSize == arrayLength) {
        changeLength1D(element, arrayLength, 2 * arrayLength);
        arrayLength *= 2;
    }

    std::copy_backward(element + theIndex, element + listSize, element + listSize + 1);

    element[theIndex] = theElement;

    listSize++;
}

template<class T>
void arrayList<T>::output(std::ostream& out) const {
    copy(element, element + listSize, std::ostream_iterator<T>(std::cout, " "));
}

template<class T>
std::ostream& operator<<(std::ostream& out, const arrayList<T>& x) {
    x.output(out);
    return out;
} 

