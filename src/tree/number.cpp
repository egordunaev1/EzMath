#include <tree/factory.hpp>
#include <tree/exception.hpp>
#include <tree/hash_utils.hpp>

namespace ezmath::tree {

Number::Number(bigint val)
    : m_value{std::move(val)}
{}

Number::Number(int64_t val)
    : Number{bigint{val}}
{}

std::unique_ptr<IExpr> Number::SimplifyImpl() {
    return nullptr;
}

size_t Number::HashImpl() const {
    constexpr size_t RANDOM_BASE = 17343862609448786151u;
    return hash::combine(RANDOM_BASE, m_value.Hash());
}

int Number::Sign() const {
    return m_value.Sign();
}

bool Number::IsEqualTo(const IExpr& other) const {
    if (Hash() != other.Hash()) {
        return false;
    }
    return other.Is<Number>() && (m_value == other.As<Number>()->m_value);
}

const Number::bigint& Number::Value() const noexcept { return m_value; }

Number::bigint&& Number::DetachValue() noexcept {
    return std::move(m_value);
}

std::unique_ptr<IExpr> Number::Copy() const {
    return math::number(m_value);
}

std::string Number::ToString() const {
    return m_value.ToString();
}

}