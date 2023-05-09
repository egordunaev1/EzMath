#pragma once

#include <tree/expression.hpp>
#include <tree/number.hpp>

namespace ezmath::tree {

class Power : public BaseExpression {
public:
    Power(std::unique_ptr<IExpr>&& base, std::unique_ptr<IExpr>&& exp);

    void SetBase(std::unique_ptr<IExpr>&& base);
    void SetExp(std::unique_ptr<IExpr>&& base);

    const IExpr& Base() const noexcept;
    const IExpr& Exp() const noexcept;

    size_t HashImpl() const override;
    bool IsConstant() const override;
    constexpr int Sign() const override { return 1; };
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;

private:
    std::unique_ptr<IExpr> m_base;
    std::unique_ptr<IExpr> m_exp;
};

}