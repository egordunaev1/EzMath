#pragma once

#include <expression_tree/expression.hpp>
#include <list>
#include <vector>

namespace ezmath::expression_tree {

class Sum : public Expression {
public:
    Sum(std::vector<std::unique_ptr<Expression>>&& values);

    const std::list<std::unique_ptr<Expression>>& Value() const noexcept;

    int Sign() const override;
    bool IsConstant() const override;
    bool IsEqualTo(const Expression& other) const override;
    std::unique_ptr<Expression> Copy() const override;
    std::string ToString() const override;
    
private:
    void Add(std::unique_ptr<Expression>&& subExpr);

private:
    std::list<std::unique_ptr<Expression>> m_value;
};

}