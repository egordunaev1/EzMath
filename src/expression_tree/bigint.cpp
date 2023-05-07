#include <expression_tree/bigint.hpp>
#include <fmt/format.h>

namespace ezmath::expression_tree {

BigInt::BigInt(const std::string_view str) {
    const auto point = str.find('.');
    if (point == std::string::npos) {
        m_dividend = impl{str};
        m_divisor = 1;
        return;
    }

    std::string dividend = fmt::format("{}{}", str.substr(0, point), str.substr(point + 1));
    const auto zeros = str.size() - point - 1;
    std::string divisor(1, '1');
    divisor.resize(1 + zeros, '0');

    m_dividend = impl{dividend};
    m_divisor = impl{divisor};
    Normalize();
}

BigInt::BigInt(impl val) 
    : m_dividend{std::move(val)}
    , m_divisor{1}
{
    Normalize();
}

BigInt::BigInt(impl dividend, impl divisor) 
    : m_dividend{std::move(dividend)}
    , m_divisor{std::move(divisor)}
{
    Normalize();
}

bool BigInt::IsInteger() const {
    return m_divisor == 1;
}

int BigInt::Sign() const {
    return m_dividend.sign();
}

std::string BigInt::ToString() const {
    if (m_divisor == 1) {
        return m_dividend.convert_to<std::string>();
    }
    return fmt::format("\\frac{{{}}}{{{}}}", m_dividend.convert_to<std::string>(), m_divisor.convert_to<std::string>());
}

void BigInt::Normalize() {
    if (m_divisor.sign() == -1) {
        m_divisor *= -1;
        m_dividend *= -1;
    }
    if (impl gcd = boost::multiprecision::gcd(m_dividend, m_divisor); gcd != 1) {
        m_dividend /= gcd;
        m_divisor /= gcd;
    }
}

BigInt BigInt::operator-() const {
    return {m_dividend * -1, m_divisor};
}

BigInt BigInt::operator*(const BigInt& other) const {
    return BigInt{*this} *= other;
}

BigInt BigInt::operator/(const BigInt& other) const {
    return BigInt{*this} /= other;
}

BigInt BigInt::operator+(const BigInt& other) const {
    return BigInt{*this} += other;
}

BigInt BigInt::operator-(const BigInt& other) const {
    return BigInt{*this} -= other;
}

BigInt& BigInt::operator*=(const BigInt& other) {
    m_dividend *= other.m_dividend;
    Normalize();
    return *this;
}

BigInt& BigInt::operator/=(const BigInt& other) {
    m_dividend /= other.m_dividend;
    Normalize();
    return *this;
}

BigInt& BigInt::operator+=(const BigInt& other) {
    impl lcm = boost::multiprecision::lcm(m_divisor, other.m_divisor);
    m_dividend = m_dividend * (m_divisor / lcm) + other.m_dividend * (other.m_divisor / lcm);
    m_divisor = std::move(lcm);
    Normalize();
    return *this;
}

BigInt& BigInt::operator-=(const BigInt& other) {
    impl lcm = boost::multiprecision::lcm(m_divisor, other.m_divisor);
    m_dividend = m_dividend * (m_divisor / lcm) - other.m_dividend * (other.m_divisor / lcm);
    m_divisor = std::move(lcm);
    Normalize();
    return *this;
}

std::optional<BigInt> pow(const BigInt& base, const BigInt& exp) {
    if ((exp.m_divisor == 1) && (exp.m_dividend <= std::numeric_limits<int>::max())) {
        const auto power = exp.m_dividend.convert_to<int>();
        return BigInt{
            boost::multiprecision::pow(base.m_dividend, power),
            boost::multiprecision::pow(base.m_divisor, power),
        };
    }
    return std::nullopt;
}

}