#pragma once

#include <expression_tree/expression.hpp>
#include <list>
#include <vector>

namespace ezmath::expression_tree {

class Product : public Expression {
public:
    Product(std::vector<std::unique_ptr<Expression>>&& values);

    const std::list<std::unique_ptr<Expression>>&  Value() const noexcept;

    bool IsConstant() const override;
    int Sign() const override;
    bool IsEqualTo(const Expression& other) const override;
    std::unique_ptr<Expression> Copy() const override;
    std::string ToString() const override;

private:
    void Add(std::unique_ptr<Expression>&& subExpr);

    void ToString(std::string& res, const Expression& add) const;
    std::string ToString(const std::vector<std::reference_wrapper<Expression>>& expressions) const;

private:
    std::list<std::unique_ptr<Expression>> m_value;
};

}