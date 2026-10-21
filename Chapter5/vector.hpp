#include "linearList.hpp"
#include <sstream>
#include <vector>

template<class T>
class vectorList : public linearList<T> {
public:
    //构造函数，复制构造函数和析构函数
    vectorList(int initialCapacity = 10);
    vectorList(const vectorList<T>&);
    ~vectorList() { delete element; }

    //ADT方法
    bool empty() const { return element->empty(); }
    int size() const { return (int) element->size(); }
    T& get(int theIndex) const;
    int indexOf(const T& theElement) const;
    void erase(int theIndex);
    void insert(int theIndex, const T& theElement);
    void output(std::ostream& out) const;

    //增加的方法
    int capacity() const { return (int) element->capacity(); }

    //线性表的其实和结束位置的迭代器
    typedef typename std::vector<T>::iterator iterator;
    iterator begin() { return element->begin(); }
    iterator end() { return element->end(); }

protected: //增加的成员
    void checkIndex(int theIndex) const;
    std::vector<T>* element; //存储线性表元素的向量
};

//vectorList的构造函数
template<class T>
vectorList<T>::vectorList(int initialCapacity) {
    //构建函数
    if (initialCapacity < 1) {
        std::ostringstream s;
        s << "Initial capacity = " << initialCapacity << " Must be > 0";
        throw illegalParameterValue(s.str());
    }

    element = new std::vector<T>;
    //创建容量为0的空向量
    element->reserve(initialCapacity);

}

template<class T>
vectorList<T>::vectorList(const vectorList<T>& theList) {
    //复制构造函数
    element = new vectorList<T>(*theList.element);
}

//vectorList的删除和插入
template<class T>
void vectorList<T>::erase(int theIndex) {
    //删除索引为theIndex的元素
    checkIndex(theIndex);
    element->erase(begin() + theIndex);
}

template<class T>
void vectorList<T>::insert(int theIndex, const T& theElement) {
    if (theIndex < 0 || theIndex > size()) {
        std::ostringstream s;
        s << "index = " << theIndex << "size = " << size();
        throw illegalIndex(s.str());
    }
}
