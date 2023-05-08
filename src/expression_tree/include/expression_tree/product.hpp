#pragma once

#include <expression_tree/expression.hpp>
#include <list>
#include <vector>

namespace ezmath::expression_tree {

class Product : public IExpr {
public:
    Product(std::vector<std::unique_ptr<IExpr>>&& values);

    const std::list<std::unique_ptr<IExpr>>&  Value() const noexcept;

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
    std::list<std::unique_ptr<IExpr>> m_value;
};

}