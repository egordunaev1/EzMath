#pragma once

#include <expression_tree/expression.hpp>
#include <expression_tree/bigint.hpp>

namespace ezmath::expression_tree {

class Number : public IExpr {
public:
    using bigint = BigInt;

    Number(bigint val);
    Number(uint64_t val);

    const bigint& Value() const noexcept;

    size_t Hash() const override;
    constexpr bool IsConstant() const override { return true; }
    constexpr int Sign() const override { return 1; }
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;
    
private:
    mutable size_t m_bufferedHash = 0;
    bigint m_value;
};

}