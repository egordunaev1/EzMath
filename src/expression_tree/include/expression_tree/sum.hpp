#pragma once

#include <expression_tree/expression.hpp>
#include <list>
#include <vector>

namespace ezmath::expression_tree {

class Sum : public IExpr {
public:
    Sum(std::vector<std::unique_ptr<IExpr>>&& values);

    const std::list<std::unique_ptr<IExpr>>& Value() const noexcept;
    
    size_t Hash() const override;
    int Sign() const override;
    bool IsConstant() const override;
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;
    
private:
    void Add(std::unique_ptr<IExpr>&& subExpr);

private:
    mutable size_t m_bufferedHash = 0;
    std::list<std::unique_ptr<IExpr>> m_value;
};

}