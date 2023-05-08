#pragma once

#include <expression_tree/expression.hpp>
#include <vector>

namespace ezmath::expression_tree {

class Sum : public BaseExpression {
public:
    Sum(std::vector<std::unique_ptr<IExpr>>&& values);
    
    void Set(size_t index, std::unique_ptr<IExpr>&& value);
    void Erase(size_t index);
    void AddTerm(std::unique_ptr<IExpr>&& value);
    void Add(std::unique_ptr<Sum>&& other);

    size_t Size() const noexcept;
    const IExpr& Get(size_t index) const noexcept;
    const std::vector<std::unique_ptr<IExpr>>& Value() const noexcept;

    size_t HashImpl() const override;
    int Sign() const override;
    bool IsConstant() const override;
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;
    
private:
    void Add(std::unique_ptr<IExpr>&& subExpr);

private:
    std::vector<std::unique_ptr<IExpr>> m_value;
};

}