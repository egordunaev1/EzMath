#pragma once

#include <expression_tree/expression.hpp>

namespace ezmath::expression_tree {

class Symbol : public Expression {
public:
    Symbol(std::string_view s);

    std::string_view Name() const noexcept;

    bool IsEqualTo(const Expression& other) const override;
    std::unique_ptr<Expression> Copy() const override;
    std::string ToString() const override;

private:
    std::string_view m_value;
};

}