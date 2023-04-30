#pragma once

#include <expression_tree/expression.hpp>
#include <expression_tree/number.hpp>

#include <list>
#include <vector>

namespace ezmath::expression_tree {

class Product : public IExpr {
public:
    Product(std::vector<std::unique_ptr<IExpr>>&& subExpr, Number&& coefficient = 1);
    Product();
    
    void Add(std::unique_ptr<IExpr>&& subExpr);

    static size_t TypeID() noexcept;
    size_t GetTypeID() const noexcept override;

    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;


private:
    void UpdateCoef();

    Number m_coefficient;
    std::list<std::unique_ptr<IExpr>> m_value;
};

}