#pragma once

#include <expression_tree/expression.hpp>
#include <list>
#include <vector>

namespace ezmath::expression_tree {

class Product : public Expression {
public:
    Product() = default;
    
    void Add(std::unique_ptr<Expression>&& subExpr);

    std::unique_ptr<Expression> Copy() const override;
    std::string ToString() const override;

private:
    void ToString(std::string& res, const Expression& add) const;
    std::string ToString(const std::vector<std::reference_wrapper<Expression>>& expressions) const;

private:
    std::list<std::unique_ptr<Expression>> m_value;
};

}