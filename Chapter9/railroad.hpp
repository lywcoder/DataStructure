#include "iostream"
#include "arrayQueue.hpp"

arrayQueue<int>* track;//缓冲轨道数组
int numberOfTracks;
int numberOfCars;
int itsTrack; //
int smallestCar; // 缓冲轨道中编号最小的车厢

void outputFromHoldingTrack() {
    //将编号最小的车厢从缓冲轨道移到出轨道
    //从栈中删除编号最小的车厢
    track[itsTrack].pop();
    std::cout << "Move car " << smallestCar << " from holding track " << itsTrack << " to output track" << std::endl;

    //检查所有栈的栈顶，寻找编号最小的车厢和所属的栈
    smallestCar = numberOfCars + 2;
    for (int i = 1; i <= numberOfCars; i++) {
        if (!track[i].empty() && track[i].front() < smallestCar) {
            smallestCar = track[i].front();
            itsTrack = i;
        }
    }
}

//把车厢调入一个缓冲轨道
bool putInHoldingTrack(int c) {
    //将车厢c移到一个缓冲轨道，返回false，当且仅当没有可用的缓冲轨道
    //为车厢c寻找最合适的缓冲轨道
    int bestTrack = 0, //目前最合适的缓冲轨道
        bestLast = 0; //besttrack中最后的车厢

    //扫描缓冲轨道
    for (int i = 1; i <= numberOfTracks; i++) {
        if (!track[i].empty()) {
            //缓冲轨道i不空
            int lastCar = track[i].back();
            if (c > lastCar && lastCar > bestLast) {
                //缓冲轨道i的尾部具有编号更大的车厢
                bestLast = lastCar;
                bestTrack = i;
            }
        }
        else {
            //缓冲轨道i为空
            if (bestTrack == 0)  {
                bestTrack = i;
            }
        }
    }

    if (bestTrack == 0) return false; //没有可以使用的缓冲轨道

    //把轨道c移到besttrack
    track[bestTrack].push(c);
    std::cout << "Move Car " << c << " from input track " << "to holding track" << bestTrack << std::endl;

    //如果需要，更新smallestcar和itstrack
    if (c < smallestCar) {
        smallestCar = c;
        itsTrack = bestTrack;
    }
    return true;
}