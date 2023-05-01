#include <expression_tree/number.hpp>
#include <expression_tree/exception.hpp>
#include <expression_tree/factory.hpp>

namespace ezmath::expression_tree {

Number::Number(const int64_t val) : m_value{val} {}
Number::Number(bigint&& val) : m_value{std::move(val)} {}
Number::Number(const bigint& val) : m_value{val} {}
Number::Number(const std::string& val) : m_value{val} {}

std::unique_ptr<IExpr> Number::Copy() const {
    return Factory::MakeNumber(m_value);
}

std::string Number::ToString() const {
    return m_value.to_string();
}

const Number::bigint& Number::GetValue() const noexcept {
    return m_value;
}

Number Number::Power(const bigint &other) const {
    if (other <= std::numeric_limits<int>::max()) {
        return pow(m_value, other.to_int());
    }
    throw exception::CalcException{"The exponent is too high"};
}

Number& Number::operator+=(const bigint& other) { m_value += other; return *this; }
Number& Number::operator-=(const bigint& other) { m_value -= other; return *this; }
Number& Number::operator*=(const bigint& other) { m_value *= other; return *this; }
Number& Number::operator/=(const bigint& other) { m_value /= other; return *this; }

Number Number::operator+(const bigint& other) { return m_value + other; }
Number Number::operator-(const bigint& other) { return m_value - other; }
Number Number::operator*(const bigint& other) { return m_value * other; }
Number Number::operator/(const bigint& other) { return m_value / other; }

}