#pragma once

#include <tree/expression.hpp>
#include <tree/number.hpp>

namespace ezmath::tree {

class Power : public BaseExpression {
public:
    Power(std::unique_ptr<IExpr>&& base, std::unique_ptr<IExpr>&& exp);

    const IExpr& GetBase() const noexcept;
    const IExpr& Exp() const noexcept;

    std::unique_ptr<IExpr>&& DetachBase() noexcept;
    std::unique_ptr<IExpr>&& DetachExp() noexcept;

    bool IsConstant() const override;
    constexpr int Sign() const override { return 1; };
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;

private:
    std::unique_ptr<IExpr> simplify_ProductBase();
    std::unique_ptr<IExpr> simplify_SimplifyChildren();
    std::unique_ptr<IExpr> simplify_DegenerateCases();

    std::unique_ptr<IExpr> SimplifyImpl() override;
    size_t HashImpl() const override;

private:
    std::unique_ptr<IExpr> m_base;
    std::unique_ptr<IExpr> m_exp;
};

}