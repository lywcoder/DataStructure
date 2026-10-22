#ifndef CHAPTER8_BRACKET_HPP
#define CHAPTER8_BRACKET_HPP

#include "arraystack.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct BracketMatchResult {
    std::vector<std::pair<int, int>> pairs; // (左位置,右位置)，按右括号到达顺序。
    std::vector<int> unmatchedLeft;
    std::vector<int> unmatchedRight;
    bool matched() const { return unmatchedLeft.empty() && unmatchedRight.empty(); }
};

inline bool isLeftBracket(char c) { return c == '(' || c == '[' || c == '{'; }
inline bool isRightBracket(char c) { return c == ')' || c == ']' || c == '}'; }
inline bool bracketPair(char left, char right) {
    return (left == '(' && right == ')') ||
           (left == '[' && right == ']') ||
           (left == '{' && right == '}');
}

// 只保存尚未匹配的左括号。右括号必须和当前栈顶类型相同。
inline BracketMatchResult matchBrackets(const std::string& expression) {
    if (expression.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::length_error("expression is too long");
    arrayStack<std::pair<char, int>> opened;
    BracketMatchResult result;
    for (std::size_t p = 0; p < expression.size(); ++p) {
        const char c = expression[p];
        const int position = static_cast<int>(p); // 位置从 0 开始。
        if (isLeftBracket(c)) {
            opened.push(std::make_pair(c, position));
        } else if (isRightBracket(c)) {
            if (opened.empty() || !bracketPair(opened.top().first, c)) {
                // 类型不符时保留栈顶，可能遇到它真正的右括号。
                result.unmatchedRight.push_back(position);
            } else {
                result.pairs.push_back(std::make_pair(opened.top().second, position));
                opened.pop();
            }
        }
    }
    while (!opened.empty()) {
        result.unmatchedLeft.push_back(opened.top().second);
        opened.pop();
    }
    std::sort(result.unmatchedLeft.begin(), result.unmatchedLeft.end());
    return result;
}

inline void printMatchedPairs(const std::string& expression, std::ostream& out) {
    const BracketMatchResult result = matchBrackets(expression);
    for (const std::pair<int, int>& pair : result.pairs)
        out << "match " << pair.first << ' ' << pair.second << '\n';
    for (int i : result.unmatchedLeft) out << "unmatched left at " << i << '\n';
    for (int i : result.unmatchedRight) out << "unmatched right at " << i << '\n';
}
inline void printMatchedPairs(const std::string& expression) {
    printMatchedPairs(expression, std::cout);
}

#endif
