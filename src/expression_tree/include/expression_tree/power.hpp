#pragma once

#include <expression_tree/expression.hpp>
#include <expression_tree/number.hpp>

namespace ezmath::expression_tree {

class Power : public IExpr {
public:
    Power(std::unique_ptr<IExpr>&& base, std::unique_ptr<IExpr>&& exp);

    static size_t TypeID() noexcept;
    size_t GetTypeID() const noexcept override;

    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;

private:
    std::unique_ptr<IExpr> m_base;
    std::unique_ptr<IExpr> m_exp;
};

}