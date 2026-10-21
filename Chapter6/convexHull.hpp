#ifndef CHAPTER6_CONVEX_HULL_HPP
#define CHAPTER6_CONVEX_HULL_HPP

#include "chain.hpp"
#include "doublyLinkedList.hpp"

#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <vector>

struct Point {
    long long x;
    long long y;
    Point(long long xValue = 0, long long yValue = 0) : x(xValue), y(yValue) {}
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

inline std::ostream& operator<<(std::ostream& out, const Point& p) {
    return out << '(' << p.x << ',' << p.y << ')';
}

// 叉积 > 0 表示 a -> b -> c 左转，< 0 表示右转，= 0 表示共线。
// 限制坐标绝对值不超过 10^9，使 long long 中间计算不会溢出。
inline long long cross(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x) * (c.y - a.y) -
           (b.y - a.y) * (c.x - a.x);
}

// Andrew 单调链。返回从字典序最小点开始、逆时针排列的凸包顶点。
// 共线的中间点不作为顶点；输入全共线时返回两个端点。
inline chain<Point> convexHull(const chain<Point>& input) {
    std::vector<Point> points(input.begin(), input.end());
    for (const Point& p : points)
        if (p.x < -1000000000LL || p.x > 1000000000LL ||
            p.y < -1000000000LL || p.y > 1000000000LL)
            throw std::out_of_range("point coordinate exceeds 10^9");

    std::sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });
    points.erase(std::unique(points.begin(), points.end()), points.end());

    chain<Point> hull;
    if (points.size() <= 2) {
        for (const Point& p : points) hull.push_back(p);
        return hull;
    }

    // 双链表当栈使用：back、secondBack、pop_back 都是 O(1)。
    doublyLinkedList<Point> lower;
    doublyLinkedList<Point> upper;
    for (const Point& p : points) {
        while (lower.size() >= 2 && cross(lower.secondBack(), lower.back(), p) <= 0)
            lower.pop_back();
        lower.push_back(p);
    }
    for (std::vector<Point>::reverse_iterator it = points.rbegin();
         it != points.rend(); ++it) {
        while (upper.size() >= 2 && cross(upper.secondBack(), upper.back(), *it) <= 0)
            upper.pop_back();
        upper.push_back(*it);
    }

    // 两条链各去掉末尾，因为该点是另一条链的起点。
    lower.pop_back();
    upper.pop_back();
    while (!lower.empty()) {
        hull.push_back(lower.front());
        lower.erase(0);
    }
    while (!upper.empty()) {
        hull.push_back(upper.front());
        upper.erase(0);
    }
    return hull;
}

#endif
