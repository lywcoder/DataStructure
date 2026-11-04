#ifndef CHAPTER1_PERM_HPP
#define CHAPTER1_PERM_HPP

#include <algorithm>
#include <iostream>
#include <iterator>

template<class T>
void permutations(T list[], int k, int m) {
    //生成list[k:m]的所有排列
    if (k == m) {
        std::copy(list, list + m + 1, std::ostream_iterator<T>(std::cout, ""));
        std::cout << std::endl;
    } else {
        for (int i = k; i <= m; i++) {
            std::swap(list[k], list[i]);
            permutations(list, k + 1, m);
            std::swap(list[k], list[i]);
        }
    }
}

#endif
