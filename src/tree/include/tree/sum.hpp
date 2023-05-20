#pragma once

#include <tree/hash_utils.hpp>
#include <tree/product.hpp>
#include <tree/expression.hpp>
#include <vector>

namespace ezmath::tree {

struct Term {
    Term(std::unique_ptr<IExpr>&& expr);

    size_t Hash;
    std::unique_ptr<IExpr> Expression;

    friend bool operator==(const Term& lhs, const Term& rhs);
};

bool operator==(const Term& lhs, const Term& rhs);

struct TermHash {
    inline size_t operator()(const Term& mul) const;
};

class Sum : public BaseExpression {
public:
    using ValueType = std::unordered_multiset<Term, TermHash>;
    using ConstantType = Number::bigint;

    Sum(std::vector<std::unique_ptr<IExpr>>&& values);

    const ConstantType& GetConstant() const noexcept;
    const ValueType& GetTerms() const noexcept;
    ConstantType&& DetachConstant() noexcept;
    ValueType&& DetachTerms() noexcept;

    size_t HashImpl() const override;
    int Sign() const override;
    bool IsConstant() const override;
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;
    
private:
    std::unique_ptr<IExpr> simplify_FactorOutCoeffs();
    std::unique_ptr<IExpr> simplify_DegenerateCases();
    std::unique_ptr<IExpr> simplify_SimplifyChildren();
    std::unique_ptr<IExpr> simplify_FactorOutTerms();
    std::unique_ptr<IExpr> simplify_AddLikeTerms();
    virtual std::unique_ptr<IExpr> SimplifyImpl() override;

    void Add(std::unique_ptr<IExpr>&& subExpr);

private:
    ConstantType m_constant;
    ValueType m_terms;
};

}