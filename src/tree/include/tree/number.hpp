#pragma once

#include <tree/expression.hpp>
#include <tree/bigint.hpp>

namespace ezmath::tree {

class Number : public BaseExpression {
public:
    using bigint = BigNum;

    Number(bigint val);
    Number(int64_t val);

    const bigint& Value() const noexcept;
    bigint&& DetachValue() noexcept;

    constexpr bool IsConstant() const override { return true; }
    int Sign() const override;
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;
    
private:
    std::unique_ptr<IExpr> SimplifyImpl() override;
    size_t HashImpl() const override;
    bigint m_value;
};

}