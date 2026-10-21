#ifndef chapter1_nextPermutation_
#define chapter1_nextPermutation_

#include <algorithm>
#include <functional>
#include <iterator>

// 教学实现，不向 std 命名空间添加函数。
namespace chapter1 {

template<class BidirectionalIterator, class Compare>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last,
                      Compare comp) {
    if (first == last)
        return false;

    BidirectionalIterator pivot = last;
    --pivot;
    if (pivot == first)
        return false;

    while (true) {
        BidirectionalIterator suffix = pivot;
        --pivot;

        // 找到最右侧的上升相邻对；suffix 起的后缀已按 comp 非递增。
        if (comp(*pivot, *suffix)) {
            BidirectionalIterator successor = last;
            // 后缀非递增，从右侧遇到的第一个更大值就是最小的可用更大值。
            do {
                --successor;
            } while (!comp(*pivot, *successor));

            std::iter_swap(pivot, successor);
            // 交换后后缀仍非递增，反转即可得到最小后缀，不需要排序。
            std::reverse(suffix, last);
            return true;
        }

        if (pivot == first) {
            // 当前已是最大排列；与 STL 一样恢复为最小排列并返回 false。
            std::reverse(first, last);
            return false;
        }
    }
}

template<class BidirectionalIterator>
bool next_permutation(BidirectionalIterator first, BidirectionalIterator last) {
    typedef typename std::iterator_traits<BidirectionalIterator>::value_type value_type;
    return chapter1::next_permutation(first, last, std::less<value_type>());
}

} // namespace chapter1

#endif
