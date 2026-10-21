#include <iostream>

int main(void) {
    int numberOfRows = 5;

    //定义每一行的长度
    int length[5] = {6, 3, 4, 2, 7};

    //声明一个二维数组的变量
    int** irregularArray = new int* [numberOfRows];
    
    //分配每一行的空间
    for (int i = 0; i < numberOfRows; i++) {
        irregularArray[i] = new int [length[i]];
    }

    //像使用规则数组一样使用不规则数组
    irregularArray[2][3] = 5;
    irregularArray[4][6] = irregularArray[2][3] + 2;
    irregularArray[1][1] = 3;

    //输出选择的数组元素
    std::cout << irregularArray[2][3] << std::endl;
    std::cout << irregularArray[4][6] << std::endl;
    std::cout << irregularArray[1][1] << std::endl;

    return 0;
}
