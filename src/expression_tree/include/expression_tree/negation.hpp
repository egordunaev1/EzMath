#pragma once

#include <expression_tree/expression.hpp>

namespace ezmath::expression_tree {

class Negation : public IExpr {
public:
    Negation(std::unique_ptr<IExpr>&& val);

    const IExpr& Value() const noexcept;

    size_t Hash() const override;
    bool IsConstant() const override;
    int Sign() const override;
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;
    
private:
    mutable size_t m_bufferedHash = 0;
    std::unique_ptr<IExpr> m_value;
};

}