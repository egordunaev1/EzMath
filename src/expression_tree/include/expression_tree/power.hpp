#pragma once

#include <expression_tree/expression.hpp>
#include <expression_tree/number.hpp>

namespace ezmath::expression_tree {

class Power : public Expression {
public:
    Power(std::unique_ptr<Expression>&& base, std::unique_ptr<Expression>&& exp);

    const Expression& Base() const noexcept;
    const Expression& Exp() const noexcept;

    std::unique_ptr<Expression> Copy() const override;
    std::string ToString() const override;

private:
    std::unique_ptr<Expression> m_base;
    std::unique_ptr<Expression> m_exp;
};

}