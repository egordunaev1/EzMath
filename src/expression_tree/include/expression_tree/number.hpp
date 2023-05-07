#pragma once

#include <expression_tree/expression.hpp>
#include <expression_tree/bigint.hpp>

namespace ezmath::expression_tree {

class Number : public IExpr {
public:
    using bigint = BigInt;

    Number(bigint val);

    const bigint& Value() const noexcept;

    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;
    
private:
    bigint m_value;
};

}