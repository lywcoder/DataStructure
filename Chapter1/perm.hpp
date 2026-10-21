#include <iostream>
#include <iterator>

template<class T>
void permutations(T list[], int k, int m) {
    //生成list[k:m]的所有排列
    if (k == m) {
        copy(list, list + m + 1, std::ostream_iterator<T>(std::cout, ""));
        std::cout << std::endl;
    } else {
        for (int i = k; i <= m; i++) {
            swap(list[k], list[i]);
            permutations(list, k + 1, m);
            swap(list[k], list[i]);
        }
    }
}