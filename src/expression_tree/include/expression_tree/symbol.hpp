#pragma once

#include <expression_tree/expression.hpp>

namespace ezmath::expression_tree {

class Symbol : public IExpr {
public:
    Symbol(std::string_view s);

    static size_t TypeID() noexcept;
    size_t GetTypeID() const noexcept override;

    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;

private:
    std::string_view m_value;
};

}