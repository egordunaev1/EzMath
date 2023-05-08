#pragma once

#include <expression_tree/expression.hpp>
#include <numeric>
#include <ranges>
#include <vector>

namespace ezmath::expression_tree::hash {

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

struct Wrapper {
    inline Wrapper() {}
    inline Wrapper(IExpr* val) : Value(val) {}

    IExpr* Value;

    inline friend bool operator==(const Wrapper& lhs, const Wrapper& rhs) { return lhs.Value->IsEqualTo(*rhs.Value); }
};

struct Hasher {
    inline size_t operator()(const Wrapper& wrapper) const { return wrapper.Value->Hash(); }
};

inline size_t asymmetric_hash(const size_t base, const std::vector<std::unique_ptr<IExpr>>& v) {
    auto hash = [](const auto& val) { return val->Hash(); };
    auto range = std::ranges::views::transform(v, hash);
    std::vector<size_t> hashes{range.begin(), range.end()};
    std::ranges::sort(hashes);
    return std::accumulate(hashes.begin(), hashes.end(), base, _combine);
}

}