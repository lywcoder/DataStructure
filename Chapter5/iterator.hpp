#ifndef CHAPTER5_ITERATOR_HPP
#define CHAPTER5_ITERATOR_HPP

// 数组连续存储，T* 本身就是随机访问迭代器。
// 它支持解引用、前进、后退、相减和下标运算。
template <class T>
using arrayListIterator = T*;

#endif
