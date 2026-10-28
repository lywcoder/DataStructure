#ifndef CHAPTER10_LZW_HPP
#define CHAPTER10_LZW_HPP

#include "hashFunctions.hpp"
#include "linearProbingHashTable.hpp"
#include <stdexcept>
#include <string>
#include <vector>

// 字节版 LZW：0..255 是单字节初始词条，后续词条按发现顺序编号。
// 返回整数码流，不做 9/10/11/12 位打包，也不引入清表码。
inline std::vector<int> lzwCompress(const std::string& input, int maxCodes = 4096) {
    if (maxCodes < 256) throw std::invalid_argument("maxCodes must be >= 256");
    linearProbingHashTable<std::string, int, Fnv1aStringHash> phrases(521, 0.7);
    for (int byte = 0; byte < 256; ++byte)
        phrases.insert({std::string(1, static_cast<char>(byte)), byte});
    if (input.empty()) return {};

    int nextCode = 256;
    std::string current(1, input[0]);
    std::vector<int> codes;
    for (std::size_t i = 1; i < input.size(); ++i) {
        const char nextByte = input[i];
        std::string extended = current;
        extended.push_back(nextByte);
        if (phrases.find(extended) != nullptr) {
            current.swap(extended); // 已知短语，继续尝试更长的串。
            continue;
        }
        codes.push_back(phrases.find(current)->second);
        if (nextCode < maxCodes) phrases.insert({extended, nextCode++});
        current.assign(1, nextByte);
    }
    codes.push_back(phrases.find(current)->second);
    return codes;
}

inline std::string lzwDecompress(const std::vector<int>& codes, int maxCodes = 4096) {
    if (maxCodes < 256) throw std::invalid_argument("maxCodes must be >= 256");
    if (codes.empty()) return {};
    if (codes[0] < 0 || codes[0] >= 256)
        throw std::invalid_argument("first LZW code must be a byte");
    std::vector<std::string> phrases;
    for (int byte = 0; byte < 256; ++byte)
        phrases.push_back(std::string(1, static_cast<char>(byte)));

    std::string previous = phrases[static_cast<std::size_t>(codes[0])];
    std::string output = previous;
    for (std::size_t i = 1; i < codes.size(); ++i) {
        const int code = codes[i];
        std::string entry;
        if (code >= 0 && static_cast<std::size_t>(code) < phrases.size()) {
            entry = phrases[static_cast<std::size_t>(code)];
        } else if (code >= 0 && static_cast<std::size_t>(code) == phrases.size() &&
                   phrases.size() < static_cast<std::size_t>(maxCodes)) {
            // 唯一合法的“尚未建表就引用下一码”：KwKwK 情况。
            entry = previous + previous[0];
        } else {
            throw std::invalid_argument("invalid LZW code sequence");
        }
        output += entry;
        // 解码器在读到下一短语时，才知道新词条的最后一个字节。
        if (phrases.size() < static_cast<std::size_t>(maxCodes))
            phrases.push_back(previous + entry[0]);
        previous.swap(entry);
    }
    return output;
}

#endif
