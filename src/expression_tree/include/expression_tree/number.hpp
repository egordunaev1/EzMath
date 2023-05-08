#pragma once

#include <expression_tree/expression.hpp>
#include <expression_tree/bigint.hpp>

namespace ezmath::expression_tree {

class Number : public Expression {
public:
    using bigint = BigInt;

    Number(bigint val);
    Number(uint64_t val);

    const bigint& Value() const noexcept;

    constexpr bool IsConstant() const override { return true; }
    constexpr int Sign() const override { return 1; }
    bool IsEqualTo(const Expression& other) const override;
    std::unique_ptr<Expression> Copy() const override;
    std::string ToString() const override;
    
private:
    bigint m_value;
};

}