#include <tree/exception.hpp>
#include <tree/bigint.hpp>
#include <tree/hash_utils.hpp>
#include <fmt/format.h>

namespace ezmath::tree {

BigNum::BigNum()
    : BigNum{0} 
{}

BigNum::BigNum(int64_t val)
    : m_value{val}
{}

BigNum::BigNum(const std::string_view str) {
    const auto point = str.find('.');
    if (point == std::string::npos) {
        m_value = Rational{str};
        return;
    }

    std::string dividend = fmt::format("{}{}", str.substr(0, point), str.substr(point + 1));
    const auto zeros = str.size() - point - 1;
    std::string divisor(1, '1');
    divisor.resize(1 + zeros, '0');

    m_value = Rational{dividend, divisor};
}

BigNum::BigNum(Rational val) 
    : m_value{std::move(val)}
{}

size_t BigNum::Hash() const {
    return boost::multiprecision::hash_value(m_value);
}

bool BigNum::IsInteger() const {
    return boost::multiprecision::denominator(m_value) == 1;
}

int BigNum::Sign() const {
    return m_value.sign();
}

std::string BigNum::ToString() const {
    auto numerator = boost::multiprecision::numerator(m_value);
    auto denominator = boost::multiprecision::denominator(m_value);

    if (denominator == 1) {
        return numerator.convert_to<std::string>();
    }
    return fmt::format("\\frac{{{}}}{{{}}}", numerator.convert_to<std::string>(), denominator.convert_to<std::string>());
}

BigNum BigNum::operator-() const {
    return BigNum{-m_value};
}

BigNum BigNum::operator*(const BigNum& other) const {
    return BigNum{*this} *= other;
}

BigNum BigNum::operator/(const BigNum& other) const {
    return BigNum{*this} /= other;
}

BigNum BigNum::operator+(const BigNum& other) const {
    return BigNum{*this} += other;
}

BigNum BigNum::operator-(const BigNum& other) const {
    return BigNum{*this} -= other;
}

BigNum& BigNum::operator*=(const BigNum& other) {
    m_value *= other.m_value;
    return *this;
}

BigNum& BigNum::operator/=(const BigNum& other) {
    m_value /= other.m_value;
    return *this;
}

BigNum& BigNum::operator+=(const BigNum& other) {
    m_value += other.m_value;
    return *this;
}

BigNum& BigNum::operator-=(const BigNum& other) {
    m_value -= other.m_value;
    return *this;
}

const BigNum::Rational& BigNum::GetImpl() const noexcept {
    return m_value;
}

std::pair<BigNum::Integer, BigNum::Integer> BigNum::Decompose() const {
    return {
        boost::multiprecision::numerator(m_value),
        boost::multiprecision::denominator(m_value)
    };
}

}