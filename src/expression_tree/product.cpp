#include <expression_tree/factory.hpp>
#include <ranges>

namespace ezmath::expression_tree {

void Product::Add(std::unique_ptr<Expression>&& subExpr) {
    if (subExpr->Sign() == -1) {
        subExpr->ToggleSign();
        m_sign *= -1;
    }
    if (subExpr->Is<Product>()) {
        for (auto& val : subExpr->As<Product>()->m_value) {
            Add(std::move(val));
        }
        return;
    }
    m_isConstant &= subExpr->IsConstant();
    m_value.emplace_back(std::move(subExpr));
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
    auto prod = Factory::MakeProduct();
    for (const auto& val : m_value) {
        prod->Add(val->Copy());
    }
    return prod;
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

        auto expCopy = exp.Copy();
        expCopy->ToggleSign();
        auto newDivisor = Factory::MakePower(
            base.Copy(),
            std::move(expCopy)
        );
        divisor.emplace_back(std::move(newDivisor));
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
    const auto sign = (m_sign == 1) ? "" : "-";
    
    if (m_value.empty()) {
        return fmt::format("{}1", sign);
    }

    if (m_value.size() == 1 && !m_value.front()->Is<Power>()) {
        if (m_value.front()->Is<Sum>() && m_sign == -1) {
            return fmt::format("-({})", m_value.front()->ToString());
        }
        return fmt::format("{}{}", sign, m_value.front()->ToString());
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

    return fmt::format("{}{}{}{}", sign, constStr, delimeter, noconstStr);
}

}