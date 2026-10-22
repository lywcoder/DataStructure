#ifndef CHAPTER8_SWITCH_BOX_HPP
#define CHAPTER8_SWITCH_BOX_HPP

#include "arraystack.hpp"
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// 每个编号在盒子边界出现恰好两次，表示一根要连接的线。
// 两根线端点若交替出现（例如 A B A B），在盒内画直线式连接会相交；
// 非交叉配对必表现为不相交或嵌套，和括号匹配相同。
inline bool switchBoxRoutable(const std::vector<int>& perimeter) {
    std::unordered_map<int, int> counts;
    for (int label : perimeter)
        if (++counts[label] > 2)
            throw std::invalid_argument("each wire label must appear twice");
    for (const auto& item : counts)
        if (item.second != 2)
            throw std::invalid_argument("each wire label must appear twice");

    std::unordered_set<int> firstSeen;
    arrayStack<int> open;
    for (int label : perimeter) {
        if (firstSeen.insert(label).second) {
            open.push(label);
        } else {
            if (open.empty() || open.top() != label) return false;
            open.pop();
        }
    }
    return open.empty();
}

#endif
