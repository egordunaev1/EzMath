#pragma once

#include <expression_tree/expression.hpp>
#include <list>
#include <vector>

namespace ezmath::expression_tree {

class Sum : public IExpr {
public:
    Sum(std::vector<std::unique_ptr<IExpr>>&& subExpr);
    Sum() = default;
    
    void Add(std::unique_ptr<IExpr>&& subExpr);

    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;

private:
    std::list<std::unique_ptr<IExpr>> m_value;
};

}