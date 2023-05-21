#include <tree/math.hpp>
#include <tree/hash_utils.hpp>
#include <ranges>
#include <unordered_set>

namespace ezmath::tree {

static std::unique_ptr<IExpr> DefaultExp = math::number(1);

std::pair<std::unique_ptr<IExpr>, std::unique_ptr<IExpr>> DecomposePower(Multiplier& mul) {
    if (mul.Expression->Is<Power>()) {
        return {
            mul.Expression->As<Power>()->DetachBase(),
            mul.Expression->As<Power>()->DetachExp()
        };
    }
    return {std::move(mul.Expression), DefaultExp->Copy()};
}

const IExpr& GetBase(const Multiplier& mul) {
    if (mul.Expression->Is<Power>()) {
        return mul.Expression->As<Power>()->GetBase();
    }
    return *mul.Expression;
}

const IExpr& GetExp(const Multiplier& mul) {
    if (mul.Expression->Is<Power>()) {
        return mul.Expression->As<Power>()->GetExp();
    }
    return *DefaultExp;
}

Multiplier::Multiplier(std::unique_ptr<IExpr>&& expression)
    : Expression{std::move(expression)}
{
    Key = Expression->Is<Power>()
        ? &Expression->As<Power>()->GetBase()
        : Expression.get();
}

Product::Product(std::vector<std::unique_ptr<IExpr>>&& values) 
    : m_coefficient{1}
{
    for (auto& val : values) {
        Add(std::move(val));
    }
}

std::unique_ptr<IExpr> Product::simplify_SimplifyChildren() {
    Product res{{}};
    res.m_constants.reserve(m_constants.size());
    res.m_variables.reserve(m_variables.size());
    res.m_coefficient = m_coefficient;

    while (!m_constants.empty()) {
        auto mul = std::move(m_constants.extract(m_constants.begin()).value().Expression);
        math::simplify(mul);
        res.Add(std::move(mul));
    }

    while (!m_variables.empty()) {
        auto mul = std::move(m_variables.extract(m_variables.begin()).value().Expression);
        math::simplify(mul);
        res.Add(std::move(mul));
    }

    *this = std::move(res);
    return nullptr;
}

void MultiplyLikeTerms(Product::ValueType& multipliers,
                       const std::function<void(std::unique_ptr<IExpr>&&)>& callback)
{
    while (!multipliers.empty()) {
        auto [begin, end] = multipliers.equal_range(*multipliers.begin());

        if (std::next(begin) == end) {
            callback(std::move(multipliers.extract(begin).value().Expression));
            continue;
        }

        auto [base, exp1] = DecomposePower(multipliers.extract(begin++).value());

        std::vector<std::unique_ptr<IExpr>> exponents;
        exponents.emplace_back(std::move(exp1));

        for (auto it = begin; it != end;) {
            auto [_, exp] = DecomposePower(multipliers.extract(it++).value());
            exponents.emplace_back(std::move(exp));
        }

        std::unique_ptr<IExpr> exponent = math::add(std::move(exponents));
        math::simplify(exponent);

        std::unique_ptr<IExpr> res = math::exp(std::move(base), std::move(exponent));
        math::simplify(res);

        callback(std::move(res));
    }
}

std::unique_ptr<IExpr> Product::simplify_MultiplyLikeTerms() {
    Product res{{}};
    res.m_coefficient = std::move(m_coefficient);
    res.m_constants.reserve(m_constants.size());
    res.m_variables.reserve(m_variables.size());
    MultiplyLikeTerms(m_constants, std::bind(&Product::Add, &res, std::placeholders::_1));
    MultiplyLikeTerms(m_variables, std::bind(&Product::Add, &res, std::placeholders::_1));
    *this = std::move(res);
    return nullptr;
}

std::unique_ptr<IExpr> Product::simplify_DegenerateCases() {
    if (m_constants.empty() && m_variables.empty()) {
        return math::number(m_coefficient);
    }
    if ((m_constants.size() + m_variables.size() == 1) && (m_coefficient == 1)) {
        return m_constants.empty()
            ? std::move(m_variables.extract(m_variables.begin()).value().Expression)
            : std::move(m_constants.extract(m_constants.begin()).value().Expression);
    }
    return nullptr;
}

std::unique_ptr<IExpr> Product::SimplifyImpl() {
    decltype(&Product::SimplifyImpl) simplifyRules[] = {
        &Product::simplify_SimplifyChildren,
        &Product::simplify_DegenerateCases,
        &Product::simplify_MultiplyLikeTerms,
        &Product::simplify_DegenerateCases
    };

    for (auto rule : simplifyRules) {
        if (auto res = (*this.*rule)()) {
            return res;
        }
    }
    return nullptr;
}

void Product::Add(std::unique_ptr<IExpr>&& subExpr) {
    if (!subExpr) {
        return;
    }

    if (subExpr->Is<Product>()) {
        auto subProduct = subExpr->As<Product>();
        m_coefficient *= subProduct->m_coefficient;
        m_constants.merge(subProduct->m_constants);
        m_variables.merge(subProduct->m_variables);
        return;
    }

    if (subExpr->Is<Number>()) {
        m_coefficient *= subExpr->As<Number>()->Value();
        return;
    }

    if (subExpr->IsConstant()) {
        m_constants.emplace(std::move(subExpr));
        return;
    }

    m_variables.emplace(std::move(subExpr));
}

const Product::Coefficient& Product::GetCoefficient() const {
    return m_coefficient;
}

const Product::ConstantPart& Product::GetConstants() const {
    return m_constants;
}

const Product::VariablePart& Product::GetVariables() const {
    return m_variables;
}

Product::Coefficient&& Product::DetachCoefficient() {
    return std::move(m_coefficient);
}

Product::ConstantPart&& Product::DetachConstants() {
    return std::move(m_constants);
}

Product::VariablePart&& Product::DetachVariables() {
    return std::move(m_variables);
}

size_t Product::HashImpl() const {
    constexpr size_t RANDOM_BASE1 = 8323215160037385666u;
    constexpr size_t RANDOM_BASE2 = 6383974336372012507u;

    auto hash1 = hash::asymmetric_hash(RANDOM_BASE1, m_constants);
    auto hash2 = hash::asymmetric_hash(RANDOM_BASE2, m_variables);
    return hash::combine(m_coefficient.Hash(), hash1, hash2);
}

bool Product::IsConstant() const {
    return m_variables.empty();
}

int Product::Sign() const {
    return m_coefficient.Sign();
}

bool Product::IsEqualTo(const IExpr& other) const {
    if (Hash() != other.Hash()) {
        return false;
    }

    if (!other.Is<Product>()) {
        return false;
    }

    auto otherProduct = other.As<Product>();

    return m_coefficient == otherProduct->m_coefficient
        && m_variables == otherProduct->m_variables
        && m_constants == otherProduct->m_constants;
}

std::unique_ptr<IExpr> Product::Copy() const {
    constexpr auto copy = [](const auto& val) { return val.Expression->Copy(); };

    auto res = math::multiply();

    res->m_coefficient = m_coefficient;
    std::transform(m_constants.begin(), m_constants.end(), std::inserter(res->m_constants, res->m_constants.end()), copy);
    std::transform(m_variables.begin(), m_variables.end(), std::inserter(res->m_variables, res->m_variables.end()), copy);

    return res;
}

void Product::ToString(std::string& res, const IExpr& add, bool isSingle) const {
    auto str = add.ToString();

    bool needBrackets = add.Is<Sum>() && !isSingle;
    bool needDelimeter = !needBrackets && !str.empty() && !res.empty() && std::isdigit(str.front());

    if (needDelimeter)
        res.append(" \\cdot ");
    if (needBrackets)
        res.append("\\left(");
    res.append(str);
    if (needBrackets)
        res.append("\\right)");
}

std::string Product::ToString(const ValueType& expressions) const {
    std::vector<std::reference_wrapper<IExpr>> dividend;
    std::vector<std::unique_ptr<IExpr>> divisor;

    if (expressions.empty()) {
        return {};
    }

    for (const auto& val : expressions) {
        decltype(auto) base = GetBase(val);
        decltype(auto) exp = GetExp(val);

        if (exp.Sign() == 1) {
            dividend.emplace_back(*val.Expression);
            continue;
        }

        std::unique_ptr<IExpr> newExp = math::exp(base.Copy(), math::negate(exp.Copy()));
        math::simplify(newExp);

        auto& res = divisor.emplace_back(std::move(newExp));
        math::simplify(res);
    }

    std::string dividendStr, divisorStr;
    std::ranges::for_each(dividend, [this, &dividendStr, isSingle = (dividend.size() == 1)](const auto& expr){ ToString(dividendStr, expr.get(), isSingle); });
    std::ranges::for_each(divisor, [this, &divisorStr, isSingle = (dividend.size() == 1)](const auto& expr){ ToString(divisorStr, *expr, isSingle); });

    if (dividendStr.empty()) {
        dividendStr = "1";
    }
    
    if (!divisorStr.empty()) {
        return fmt::format("\\frac{{{}}}{{{}}}", dividendStr, divisorStr);
    }

    return dividendStr;
}

std::string Product::ToString() const {
    if (m_constants.empty() && m_variables.empty()) {
        return m_coefficient.ToString();
    }

    auto constStr = ToString(m_constants);
    auto varsStr = ToString(m_variables);

    auto coef = (m_coefficient == 1) ? "" : m_coefficient.ToString();
    if (m_coefficient == -1) {
        coef = "-";
    }
    const auto delimeter1 = std::isdigit(constStr.front()) ? " \\cdot " : "";
    const auto delimeter2 = std::isdigit(varsStr.front()) ? " \\cdot " : "";    
    
    return fmt::format("{}{}{}{}{}", coef, delimeter1, constStr, delimeter2, varsStr);
}

}