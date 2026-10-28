#ifndef CHAPTER10_HASH_FUNCTIONS_HPP
#define CHAPTER10_HASH_FUNCTIONS_HPP

#include <cstddef>
#include <cstdint>
#include <string>

// 教学用 FNV-1a：逐字节混合，结果再由散列表对桶数取模。
// 固定算法便于复现实验；它不是密码学散列函数，不防恶意碰撞。
struct Fnv1aStringHash {
    std::size_t operator()(const std::string& text) const noexcept {
        std::uint64_t hash = 14695981039346656037ULL;
        for (unsigned char byte : text) {
            hash ^= byte;
            hash *= 1099511628211ULL;
        }
        return static_cast<std::size_t>(hash);
    }
};

#endif
