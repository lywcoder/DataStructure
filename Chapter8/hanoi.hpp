// 使用递归的方法
#include "iostream"
#include "arraystack.hpp"

// void towersOfHanoi(int n, int x, int y, int z) {
//     //使用塔z作为中转地
//     if (n > 0) {
//         towersOfHanoi(n - 1, x, z, y);
//         std::cout << "Move top disk from tower " << x << " to top of tower " << y << std::endl;
//         towersOfHanoi(n - 1, z, y, x);
//     }
// }

//使用栈来求解汉诺塔问题
//全局变量，使用tower[1:3]来表示3个塔
arrayStack<int> tower[4];

void moveAndShow(int, int, int, int);

void towersOfHanoi(int n) {
    for (int d = n; d > 0; d--) {
        tower[1].push(d);
    }
    //将n个碟子从1移到3，使用塔2作为中转站
    moveAndShow(n, 1, 2, 3);
}

void moveAndShow(int n, int x, int y, int z) {
    //使用塔z作为中转站
    if (n > 0) {
        moveAndShow(n - 1, x, z, y);
        int d = tower[x].top();
        tower[x].pop();
        tower[y].push(d);
        // showState(); //显示塔3的布局
        moveAndShow(n - 1, z, y, x);
    }
}