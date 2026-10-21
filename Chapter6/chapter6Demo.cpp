#include "binSort.hpp"
#include "circularList.hpp"
#include "convexHull.hpp"
#include "disjointSet.hpp"
#include "radixSort.hpp"

#include <iostream>

int main() {
    chain<int> singly;
    singly.push_back(10);
    singly.push_back(30);
    singly.insert(1, 20); // 只改链接，不移动数组中的元素。
    std::cout << "single: " << singly << '\n';

    circularListWithHeader<int> circular;
    circular.push_back(1);
    circular.push_back(2);
    std::cout << "circular with header: " << circular << '\n';

    doublyLinkedList<int> doubly;
    doubly.push_back(4);
    doubly.push_back(5);
    doubly.push_back(6);
    doubly.pop_back();
    std::cout << "double after pop_back: " << doubly << '\n';

    chain<studentRecord> students;
    students.push_back(studentRecord(2, "Alice"));
    students.push_back(studentRecord(1, "Bob"));
    students.push_back(studentRecord(2, "Cindy"));
    binSort(students, 2); // 同分记录 Alice、Cindy 的相对顺序保持不变。
    std::cout << "bucket sort: " << students << '\n';

    chain<int> numbers;
    for (int x : {170, 45, 75, 90, 802, 24, 2, 66}) numbers.push_back(x);
    radixSort(numbers);
    std::cout << "radix sort: " << numbers << '\n';

    chain<Point> points;
    for (const Point& p : {Point(0, 0), Point(2, 0), Point(2, 2),
                           Point(0, 2), Point(1, 1)})
        points.push_back(p);
    std::cout << "convex hull: " << convexHull(points) << '\n';

    disjointSet groups(4);
    groups.unite(0, 1);
    groups.unite(2, 3);
    std::cout << "0 and 1 connected: " << groups.connected(0, 1) << '\n';
    std::cout << "0 and 3 connected: " << groups.connected(0, 3) << '\n';
}
