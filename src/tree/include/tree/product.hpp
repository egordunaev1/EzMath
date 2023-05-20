#pragma once

#include <tree/hash_utils.hpp>
#include <tree/number.hpp>
#include <unordered_set>
#include <vector>

namespace ezmath::tree {

struct Multiplier {
    Multiplier(std::unique_ptr<IExpr>&& expression);

    const IExpr* Key;
    std::unique_ptr<IExpr> Expression;

    inline friend bool operator==(const Multiplier& lhs, const Multiplier& rhs) { 
        return lhs.Key->IsEqualTo(*rhs.Key); 
    }
};

struct MultiplierHash {
    inline size_t operator()(const Multiplier& mul) const { return mul.Key->Hash(); }
};

std::pair<std::unique_ptr<IExpr>, std::unique_ptr<IExpr>> DecomposePower(Multiplier& mul);

const IExpr& GetBase(const Multiplier& mul);
const IExpr& GetExp(const Multiplier& mul);


class Product : public BaseExpression {
public:
    using ValueType = std::unordered_multiset<Multiplier, MultiplierHash>;
    using ConstantPart = ValueType;
    using VariablePart = ValueType;
    using Coefficient = Number::bigint;

    Product(std::vector<std::unique_ptr<IExpr>>&& values);

    const Coefficient& GetCoefficient() const;
    const ConstantPart& GetConstants() const;
    const VariablePart& GetVariables() const;

    Coefficient&& DetachCoefficient();
    ConstantPart&& DetachConstants();
    VariablePart&& DetachVariables();

    bool IsConstant() const override;
    int Sign() const override;
    bool IsEqualTo(const IExpr& other) const override;
    std::unique_ptr<IExpr> Copy() const override;
    std::string ToString() const override;

private:
    std::unique_ptr<IExpr> simplify_MultiplyLikeTerms();
    std::unique_ptr<IExpr> simplify_SimplifyChildren();
    std::unique_ptr<IExpr> simplify_DegenerateCases();

    std::unique_ptr<IExpr> SimplifyImpl() override;
    size_t HashImpl() const override;

    void Add(std::unique_ptr<IExpr>&& subExpr);

    void ToString(std::string& res, const IExpr& add) const;
    std::string ToString(const ValueType& expressions) const;

private:
    Coefficient m_coefficient;
    ConstantPart m_constants;
    VariablePart m_variables;
};

}