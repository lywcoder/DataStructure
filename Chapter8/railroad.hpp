#include "arraystack.hpp"

arrayStack<int> *track; //缓冲轨道数组
int numberOfCars;
int numberOfTracks;
int smallestCar; //缓冲轨道中编号最小的车厢
int itsTrack; //停靠在最小编号车厢的缓冲轨道

void outputFromHoldingTrack() {
    //从栈itstack中删除编号最小的车厢
    track[itsTrack].pop();
    std::cout << "Move car" << smallestCar << " from holding " 
        << "track " << itsTrack << " to output track" << std::endl;

    //检查所有栈的栈顶，寻找编号最小的车厢和所属的栈
    smallestCar = numberOfCars + 2;
    for (int i = 1; i <= numberOfTracks; i++) {
        if (!track[i].empty() && (track[i].top() < smallestCar)) {
            smallestCar = track[i].top();
            itsTrack = i;
        }
    }
}

bool putInHoldingTrack(int c) {
    //初始化
    int bestTrack = 0, //目前最合适的缓冲轨道
    bestTop = numberOfCars + 1; //取bestTrack中顶部的车厢

    //扫描缓冲轨道
    for (int i = 1; i <= numberOfTracks; i++) {
        if (!track[i].empty()) {
            int topCar = track[i].top();
            if (c < topCar && topCar < bestTop) {
                //缓冲轨道i的栈顶具有编号更小的车厢
                bestTop = topCar;
                bestTrack = i;
            }
        }
        else //缓冲轨道i为空
            if (bestTrack == 0) bestTrack = i;
    }

    //将车厢c移到轨道besttrack
    track[bestTrack].push(c);
    std::cout << "Move car " << c << " from input track "
        << "to holding track" << bestTrack << std::endl;

    //如果需要，更新smallestCar和itsTrack
    if (c < smallestCar) {
        smallestCar = c;
        itsTrack = bestTrack;
    }

    return true;
}

bool railroad(int inputOrder[], int theNumberOfCars, int theNumberOfTracks) {
    //使用初始顺序开始重排车厢
    // 如果重排成功，返回true，否则就是false
    // 
    numberOfCars = theNumberOfCars;
    numberOfTracks = theNumberOfTracks;
    
    //创建用于缓冲轨道的栈
    track = new arrayStack<int> [numberOfTracks + 1]; 

    int nextCarToOutput = 1;
    smallestCar = numberOfCars + 1;

    //重排车厢
    for (int i = 1; i <= numberOfCars; i++) {
        if (inputOrder[i] == nextCarToOutput) {
            std::cout << "Move car " << inputOrder[i] 
                << " from input track to output track" << std::endl;

            //从缓冲轨道移到出轨道
            while (smallestCar == nextCarToOutput) {
                outputFromHoldingTrack();
                nextCarToOutput++;
            }
        }
        
        else {
        //将车厢Inputorder[i] 移动到另一个缓冲轨道 
            if (!putInHoldingTrack(inputOrder[i])) {
                return false;
            }
        } 
    }

    return true;
}

