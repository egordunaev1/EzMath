#pragma once

#include <expression_tree/expression.hpp>

namespace ezmath::expression_tree {

class Symbol : public IExpr {
public:
    Symbol(std::string_view s);

    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;

private:
    std::string_view m_value;
};

}