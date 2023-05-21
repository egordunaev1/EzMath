#pragma once

#include <tree/expression.hpp>
#include <numeric>
#include <vector>

namespace ezmath::tree::hash {

constexpr size_t _combine(size_t lhs, size_t rhs) {
    lhs ^= rhs + 0x517cc1b727220a95 + (lhs << 6) + (lhs >> 2);
    return lhs;
}

constexpr size_t combine(size_t lhs, size_t rhs) {
    return _combine(lhs, rhs);
}

template<class... Args>
constexpr size_t combine(size_t lhs, size_t rhs, Args... args) {
    return combine(_combine(lhs, rhs), args...);
}

template<std::ranges::range T>
inline size_t asymmetric_hash(const size_t base, const T& container) {
    std::vector<size_t> hashes;
    hashes.reserve(container.size());
    for (const auto& val : container) {
        hashes.emplace_back(val.Expression->Hash());
    }
    std::sort(hashes.begin(), hashes.end());
    return std::accumulate(hashes.begin(), hashes.end(), base, _combine);
}

template<std::ranges::range T>
inline size_t asymmetric_hash(const T& container) {
    std::vector<size_t> hashes;
    hashes.reserve(container.size());
    for (const auto& val : container) {
        hashes.emplace_back(val.Expression->Hash());
    }
    std::sort(hashes.begin(), hashes.end());
    return std::accumulate(++hashes.begin(), hashes.end(), hashes.front(), _combine);
}

}