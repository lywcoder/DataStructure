//next_permutation(start, end)
//对范围内的元素，按照字典的顺序，产生下一个更大的排列，当且仅当这个排列存在的时候，返回true
// 从字典元素最小的排列顺序开始，调用next_permutation，可以得到所有的序列
// 
#include <algorithm>
#include <iostream>
#include <iterator>

template<class T>
void permutation(T list[], int k, int m) {
    do {
        copy(list, list + m + 1, std::ostream_iterator<T>(std::cout, ""));
    } while(std::next_permutation(list, list + m + 1));
}

//std::next_permutation(start, end, compare)