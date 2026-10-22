#include <iostream>
#include <vector>

// 不规则二维数组：每行可以有不同的长度，并非一个矩形矩阵。
int main() {
    const std::vector<std::size_t> lengths = {6, 3, 4, 2, 7};
    std::vector<std::vector<int>> rows;
    for (std::size_t length : lengths) rows.push_back(std::vector<int>(length, 0));

    rows[2][3] = 5;
    rows[4][6] = rows[2][3] + 2;
    rows[1][1] = 3;
    std::cout << rows[2][3] << '\n' << rows[4][6] << '\n' << rows[1][1] << '\n';
}
