#pragma once

#include <expression_tree/expression.hpp>
#include <BigInt.hpp>

namespace ezmath::expression_tree {

class Number : public IExpr {
public:
    using bigint = BigInt;

    Number() = default;
    Number(int64_t val);
    Number(bigint&& val);
    Number(const bigint& val);
    Number(const std::string& val);

    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;

    const bigint& GetValue() const noexcept;

    Number Power(const bigint& other) const;

    Number& operator+=(const bigint& other);
    Number& operator-=(const bigint& other);
    Number& operator*=(const bigint& other);
    Number& operator/=(const bigint& other);

    Number operator+(const bigint& other);
    Number operator-(const bigint& other);
    Number operator*(const bigint& other);
    Number operator/(const bigint& other);
    

private:
    bigint m_value;
};

}