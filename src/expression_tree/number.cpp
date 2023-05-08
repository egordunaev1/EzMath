#include <expression_tree/factory.hpp>
#include <expression_tree/exception.hpp>
#include <expression_tree/hash_utils.hpp>

namespace ezmath::expression_tree {

Number::Number(bigint val)
    : m_value{std::move(val)}
{
    if (val.Sign() == -1) {
        throw exception::CalcException{"constructing Number class from negative number"};
    }
}

Number::Number(uint64_t val) 
    : Number{bigint{static_cast<int64_t>(val)}}
{}

size_t Number::HashImpl() const {
    constexpr size_t RANDOM_BASE = 17343862609448786151u;
    return hash::combine(RANDOM_BASE, m_value.Hash());
}

bool Number::IsEqualTo(const IExpr& other) const {
    if (Hash() != other.Hash()) {
        return false;
    }
    return other.Is<Number>() && (m_value == other.As<Number>()->m_value);
}

void Number::SetValue(bigint val) {
    m_value = std::move(val);
    OnChange();
}

const Number::bigint& Number::Value() const noexcept { return m_value; }

std::unique_ptr<IExpr> Number::Copy() const {
    return math::number(m_value);
}

std::string Number::ToString() const {
    return m_value.ToString();
}

}