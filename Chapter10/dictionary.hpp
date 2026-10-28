#ifndef CHAPTER10_DICTIONARY_HPP
#define CHAPTER10_DICTIONARY_HPP

#include <utility>

// 字典 ADT：每个键至多有一个值。实现可以有序，也可以按哈希桶组织。
// insert 已有键时更新值；erase 不存在的键时不改变字典。
template<class K, class E>
class dictionary {
public:
    typedef std::pair<const K, E> value_type;
    virtual ~dictionary() {}
    virtual bool empty() const = 0;
    virtual int size() const = 0;
    // 保留教材接口：const 字典仍返回可写值指针；调用者不能改 const 键。
    // 指针由容器拥有，删除对应键或销毁容器后失效。
    virtual std::pair<const K, E>* find(const K&) const = 0;
    virtual void erase(const K&) = 0;
    virtual void insert(const std::pair<const K, E>&) = 0;
    bool contains(const K& key) const { return find(key) != nullptr; }
};

#endif
