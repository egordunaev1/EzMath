#pragma once

#include <tuple>
#include <optional>
#include <boost/multiprecision/cpp_int.hpp>

namespace ezmath::tree {

class BigNum {
public:
    using Rational = boost::multiprecision::cpp_rational;
    using Integer = boost::multiprecision::cpp_int;

    BigNum();
    BigNum(std::string_view str);
    BigNum(Rational val);
    BigNum(int64_t val);

    std::pair<Integer, Integer> Decompose() const;
    const Rational& GetImpl() const noexcept;

    size_t Hash() const;

    bool IsInteger() const;
    int Sign() const;
    std::string ToString() const;

    BigNum operator-() const;

    BigNum operator*(const BigNum& other) const;
    BigNum operator/(const BigNum& other) const;
    BigNum operator+(const BigNum& other) const;
    BigNum operator-(const BigNum& other) const;

    BigNum& operator*=(const BigNum& other);
    BigNum& operator/=(const BigNum& other);
    BigNum& operator+=(const BigNum& other);
    BigNum& operator-=(const BigNum& other);

    inline auto operator<=>(const BigNum& other) const {
        return std::tie(m_value) <=> std::tie(other.m_value);
    }

    inline bool operator==(const BigNum& other) const {
        return m_value == other.m_value;
    }

private:
    Rational m_value;
};

}