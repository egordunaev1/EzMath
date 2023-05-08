#include <expression_tree/factory.hpp>
#include <ranges>

namespace ezmath::expression_tree {

Product::Product(std::vector<std::unique_ptr<Expression>>&& values) {
    for (auto& val : values) {
        Add(std::move(val));
    }
}

void Product::Add(std::unique_ptr<Expression>&& subExpr) {
    if (subExpr->Is<Product>()) {
        for (auto& val : subExpr->As<Product>()->m_value) {
            Add(std::move(val));
        }
        return;
    }
    m_value.emplace_back(std::move(subExpr));
}

const std::list<std::unique_ptr<Expression>>& Product::Value() const noexcept {
    return m_value;
}

bool Product::IsConstant() const {
    return std::all_of(m_value.begin(), m_value.end(), 
        [](const auto& val){ return val->IsConstant(); });
}

int Product::Sign() const {
    int sign = 1;
    std::ranges::for_each(m_value, [&sign](const auto& val){sign *= val->Sign();});
    return sign;
}

bool Product::IsEqualTo(const Expression& other) const {
    if (!other.Is<Product>()) {
        return false;
    }
    const auto otherAsProd = other.As<Product>();
    if (otherAsProd->m_value.size() != m_value.size()) {
        return false;
    }

    bool isMatched[m_value.size()];
    std::memset(&isMatched, 0, m_value.size());
    for (const auto& val : m_value) {
        size_t index = 0;
        for (const auto& otherVal : otherAsProd->m_value) {
            if (!isMatched[index] && val->IsEqualTo(*otherVal)) {
                isMatched[index] = true;
                break;
            }
            ++index;
        }
        if (const auto found = (index == otherAsProd->m_value.size()); found) {
            return false;
        }
    }
    return true;
}

std::unique_ptr<Expression> Product::Copy() const {
    constexpr auto copy = [](const auto& val) { return val->Copy(); };

    std::vector<std::unique_ptr<Expression>> values;
    std::transform(m_value.begin(), m_value.end(), std::back_inserter(values), copy);
    return Factory::MakeProduct(std::move(values));
}

void Product::ToString(std::string& res, const Expression& add) const {
    auto str = add.ToString();

    bool needDelimeter = !str.empty() && !res.empty() && std::isdigit(str.front());
    bool needBrackets = add.Is<Sum>();

    if (needDelimeter) 
        res.append(" \\cdot ");
    if (needBrackets) 
        res.push_back('(');
    res.append(str);
    if (needBrackets)
        res.push_back(')');
}

std::string Product::ToString(const std::vector<std::reference_wrapper<Expression>>& expressions) const {
    std::vector<std::reference_wrapper<Expression>> dividend;
    std::vector<std::unique_ptr<Expression>> divisor;
    for (const auto& val : expressions) {
        if (!val.get().Is<Power>()) {
            dividend.emplace_back(val);
            continue;
        }
        
        decltype(auto) base = val.get().As<Power>()->Base();
        decltype(auto) exp = val.get().As<Power>()->Exp();
        if (exp.Sign() != -1) {
            dividend.emplace_back(val);
            continue;
        }
        divisor.emplace_back(Factory::MakePower(base.Copy(), Factory::Negate(exp.Copy())));
    }

    if (dividend.empty() && divisor.empty()) {
        return {};
    }

    std::string dividendStr, divisorStr;
    std::ranges::for_each(dividend, [this, &dividendStr](const auto& expr){ ToString(dividendStr, expr.get()); });
    std::ranges::for_each(divisor, [this, &divisorStr](const auto& expr){ ToString(divisorStr, *expr); });


    if (dividendStr.empty())
        dividendStr = "1";
    if (!divisorStr.empty()) {
        return fmt::format("\\frac{{{}}}{{{}}}", dividendStr, divisorStr);
    }
    return dividendStr;
}

std::string Product::ToString() const {
    if (m_value.empty()) {
        return "1";
    }

    if (m_value.size() == 1 && !m_value.front()->Is<Power>()) {
        return m_value.front()->ToString();
    }

    std::vector<std::reference_wrapper<Expression>> coefPart;
    std::vector<std::reference_wrapper<Expression>> varPart;

    for (const auto& val : m_value) {
        if (val->IsConstant()) {
            coefPart.emplace_back(*val);
        } else {
            varPart.emplace_back(*val);
        }
    }

    auto constStr = ToString(coefPart);
    auto noconstStr = ToString(varPart);

    if (constStr.empty())
        return noconstStr;
    if (noconstStr.empty()) {
        return constStr;
    }

    const auto delimeter = std::isdigit(noconstStr.front()) ? " \\cdot " : "";
    return fmt::format("{}{}{}", constStr, delimeter, noconstStr);
}

}