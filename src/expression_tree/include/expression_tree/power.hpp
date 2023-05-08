#pragma once

#include <expression_tree/expression.hpp>
#include <expression_tree/number.hpp>

namespace ezmath::expression_tree {

class Power : public IExpr {
public:
    Power(std::unique_ptr<IExpr>&& base, std::unique_ptr<IExpr>&& exp);

    const IExpr& Base() const noexcept;
    const IExpr& Exp() const noexcept;

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