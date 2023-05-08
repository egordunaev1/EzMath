#pragma once

#include <tuple>
#include <optional>
#include <boost/multiprecision/cpp_int.hpp>

namespace ezmath::expression_tree {

class BigInt {
public:
    using impl = boost::multiprecision::cpp_int;

    BigInt(std::string_view str);
    BigInt(impl val);
    BigInt(int64_t val);

    size_t Hash() const;

    bool IsInteger() const;
    int Sign() const;
    std::string ToString() const;

    BigInt operator-() const;

    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;

    BigInt& operator*=(const BigInt& other);
    BigInt& operator/=(const BigInt& other);
    BigInt& operator+=(const BigInt& other);
    BigInt& operator-=(const BigInt& other);

    inline auto operator<=>(const BigInt& other) const {
        auto lcm = boost::multiprecision::lcm(m_divisor, other.m_divisor);
        auto lval = m_dividend * (m_divisor / lcm);
        auto rval = other.m_dividend * (other.m_divisor / lcm);
        auto otmp = other.ToString();
        auto tmp0 = ToString();
        auto tmp1 = lval.convert_to<std::string>();
        auto tmp2 = rval.convert_to<std::string>();
        return std::tie(lval) <=> std::tie(rval);
    }

    inline bool operator==(const BigInt& other) const {
        return (*this <=> other) == std::weak_ordering::equivalent;
    }

    friend std::optional<BigInt> pow(const BigInt& base, const BigInt& exp);
    
private:
    BigInt(impl dividend, impl divisor);
    void Normalize();

private:
    impl m_dividend;
    impl m_divisor;
};

std::optional<BigInt> pow(BigInt base, BigInt exp);

}