#pragma once

#include <expression_tree/expression.hpp>

namespace ezmath::expression_tree {

class Negation : public Expression {
public:
    Negation(std::unique_ptr<Expression>&& val);

    const Expression& Value() const noexcept;

    bool IsConstant() const override;
    int Sign() const override;
    bool IsEqualTo(const Expression& other) const override;
    std::unique_ptr<Expression> Copy() const override;
    std::string ToString() const override;
    
private:
    std::unique_ptr<Expression> m_value;
};

}