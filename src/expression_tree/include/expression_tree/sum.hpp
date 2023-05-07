#pragma once

#include <expression_tree/expression.hpp>
#include <list>

namespace ezmath::expression_tree {

class Sum : public Expression {
public:
    Sum();
    
    void Add(std::unique_ptr<Expression>&& subExpr);

    std::unique_ptr<Expression> Copy() const override;
    std::string ToString() const override;

private:
    void ToString(std::string& res, Expression& expr) const;

private:
    std::list<std::unique_ptr<Expression>> m_value;
};

}