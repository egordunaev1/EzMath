#pragma once

#include <tree/expression.hpp>
#include <tree/bigint.hpp>

namespace ezmath::tree {

class Number : public BaseExpression {
public:
    using bigint = BigInt;

    Number(bigint val);
    Number(uint64_t val);

    void SetValue(bigint val);

    const bigint& Value() const noexcept;

    size_t HashImpl() const override;
    constexpr bool IsConstant() const override { return true; }
    constexpr int Sign() const override { return 1; }
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;
    
private:
    bigint m_value;
};

}