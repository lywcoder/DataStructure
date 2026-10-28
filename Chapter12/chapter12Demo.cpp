#include "maxheap.hpp"
#include "maxHBLT.hpp"
#include "heapSort.hpp"
#include "huffmanCoding.hpp"
#include "machineScheduling.hpp"
#include <iostream>
#include <vector>

int main() {
    maxHeap<int> heap(std::vector<int>{4, 10, 3, 5, 1});
    heap.push(12);
    std::cout << "max heap pop order: ";
    while (!heap.empty()) {
        std::cout << heap.top() << ' ';
        heap.pop();
    }
    std::cout << '\n';

    maxHBLT<int> leftistA(std::vector<int>{10, 4, 8});
    maxHBLT<int> leftistB(std::vector<int>{12, 5, 3});
    leftistA.merge(leftistB); // B 的结点所有权转入 A
    leftistA.push(15);
    std::cout << "HBLT merged top: " << leftistA.top()
              << ", size: " << leftistA.size()
              << ", other empty: " << leftistB.empty() << '\n';
    leftistA.pop();
    std::cout << "HBLT after pop: " << leftistA.top() << '\n';

    std::vector<int> values{4, 10, 3, 5, 1};
    heapSort(values);
    std::cout << "heap sort: ";
    for (int value : values) std::cout << value << ' ';
    std::cout << '\n';

    const std::string text = "aaabbc";
    huffmanCoding coding(text);
    const std::string bits = coding.encode(text);
    std::cout << "Huffman bits: " << bits << ", decoded: " << coding.decode(bits) << '\n';

    const schedulingResult schedule = scheduleJobs(
        {{1, 7}, {2, 6}, {3, 5}, {4, 4}, {5, 3}, {6, 2}, {7, 1}}, 3);
    for (const schedulingAssignment& a : schedule.assignments)
        std::cout << "job " << a.jobId << " machine " << a.machineId
                  << " [" << a.start << ',' << a.finish << ")\n";
    std::cout << "makespan: " << schedule.makespan << '\n';
}
