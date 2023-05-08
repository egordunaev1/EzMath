#pragma once

#include <expression_tree/expression.hpp>
#include <vector>

namespace ezmath::expression_tree {

class Product : public BaseExpression {
public:
    Product(std::vector<std::unique_ptr<IExpr>>&& values);

    void Set(size_t index, std::unique_ptr<IExpr>&& value);
    void Erase(size_t index);
    void AddMultiplier(std::unique_ptr<IExpr>&& value);
    void Multiply(std::unique_ptr<Product>&& other);

    size_t Size() const noexcept;
    const IExpr& Get(size_t index) const noexcept;
    std::unique_ptr<Product> Coefficient() const noexcept;
    std::unique_ptr<Product> Variable() const noexcept;
    const std::vector<std::unique_ptr<IExpr>>& Value() const noexcept;

    size_t HashImpl() const override;
    bool IsConstant() const override;
    int Sign() const override;
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;

private:
    void Add(std::unique_ptr<IExpr>&& subExpr);

    void ToString(std::string& res, const IExpr& add) const;
    std::string ToString(const std::vector<std::reference_wrapper<IExpr>>& expressions) const;

private:
    std::vector<std::unique_ptr<IExpr>> m_value;
};

}