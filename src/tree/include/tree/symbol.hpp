#pragma once

#include <tree/expression.hpp>

namespace ezmath::tree {

class Symbol : public BaseExpression {
public:
    Symbol(std::string_view s);

    std::string_view Name() const noexcept;

    constexpr bool IsConstant() const override { return false; }
    constexpr int Sign() const override { return 1; }
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;

private:
    std::unique_ptr<IExpr> SimplifyImpl() override;
    size_t HashImpl() const override;
    std::string_view m_value;
};

}