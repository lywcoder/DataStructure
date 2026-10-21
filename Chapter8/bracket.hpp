#include <iostream>
#include <ppltasks.h>
#include "arraystack.hpp"

void printMatchedPairs(std::string expr) {
    //括号匹配
    arrayStack<int> s;
    int length = (int) expr.size();

    //扫描表达式expr中的左括号和右括号
    for (int i = 0; i < length; i++) {
        if (expr.at(i) == '(') s.push(i);
        else 
            if (expr.at(i) == ')')
        try {
            //从栈中删除匹配的左括号
            std::cout << s.top() << ' ' << i << std::endl;
            s.pop();
        }
        catch (stackEmpty) {
            //栈为空，没有匹配的左括号
            std::cout << "No match for tight paranthesis"
                << "at " << i << std::endl;
        }
    }
    //栈如果不为空，剩余在栈中的左括号是不匹配的
    while (!s.empty()) {
        std::cout << "No match for left paraenthesis at" << s.top() << std::endl;
        s.pop();
    }
}