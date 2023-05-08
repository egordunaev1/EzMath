
#include <cstddef>

namespace ezmath::hash {

constexpr size_t combine(size_t lhs, size_t rhs) {
    lhs ^= rhs + 0x517cc1b727220a95 + (lhs << 6) + (lhs >> 2);
    return lhs;
}

template<class... Args>
constexpr size_t combine(size_t lhs, size_t rhs, Args... args) {
    return combine(combine(lhs, rhs), args...);
}

}