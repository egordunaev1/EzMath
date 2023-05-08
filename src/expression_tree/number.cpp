#include <expression_tree/factory.hpp>
#include <expression_tree/exception.hpp>

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

bool Number::IsEqualTo(const Expression& other) const {
    return other.Is<Number>() && (m_value == other.As<Number>()->m_value);
}

const Number::bigint& Number::Value() const noexcept { return m_value; }

std::unique_ptr<Expression> Number::Copy() const {
    return math::number(m_value);
}

std::string Number::ToString() const {
    return m_value.ToString();
}

}