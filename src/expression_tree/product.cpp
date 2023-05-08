#include <expression_tree/factory.hpp>
#include <expression_tree/hash_utils.hpp>
#include <ranges>
#include <unordered_set>

namespace ezmath::expression_tree {

Product::Product(std::vector<std::unique_ptr<IExpr>>&& values) {
    for (auto& val : values) {
        Add(std::move(val));
    }
}

void Product::Add(std::unique_ptr<IExpr>&& subExpr) {
    if (subExpr->Is<Product>()) {
        for (auto& val : subExpr->As<Product>()->m_value) {
            Add(std::move(val));
        }
        return;
    }
    m_value.emplace_back(std::move(subExpr));
}

const std::list<std::unique_ptr<IExpr>>& Product::Value() const noexcept {
    return m_value;
}

size_t Product::Hash() const {
    constexpr size_t RANDOM_BASE = 8323215160037385666u;

    if (m_bufferedHash) {
        return m_bufferedHash;
    }
    
    size_t result = RANDOM_BASE;
    for (const auto& val : m_value) {
        hash::combine(result, val->Hash());
    }
    return m_bufferedHash = result;
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

struct Wrapper {
    Wrapper() {}
    Wrapper(IExpr* val) : Value(val) {}

    IExpr* Value;

    friend bool operator==(const Wrapper& lhs, const Wrapper& rhs) { return lhs.Value->IsEqualTo(*rhs.Value); }
};

struct Hasher {
    size_t operator()(const Wrapper& wrapper) const { return wrapper.Value->Hash(); }
};

bool Product::IsEqualTo(const IExpr& other) const {
    if (Hash() != other.Hash()) {
        return false;
    }
    if (!other.Is<Product>()) {
        return false;
    }

    const auto otherAsProd = other.As<Product>();
    if (otherAsProd->m_value.size() != m_value.size()) {
        return false;
    }

    std::unordered_multiset<Wrapper, Hasher> lhs;
    std::unordered_multiset<Wrapper, Hasher> rhs;

    lhs.reserve(m_value.size());
    rhs.reserve(m_value.size());

    for (const auto& val : m_value)
        lhs.insert(val.get());
    for (const auto& val : otherAsProd->m_value)
        rhs.insert(val.get());

    return lhs == rhs;
}

std::unique_ptr<IExpr> Product::Copy() const {
    constexpr auto copy = [](const auto& val) { return val->Copy(); };

    std::vector<std::unique_ptr<IExpr>> values;
    std::transform(m_value.begin(), m_value.end(), std::back_inserter(values), copy);
    return math::multiply(std::move(values));
}

void Product::ToString(std::string& res, const IExpr& add) const {
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

std::string Product::ToString(const std::vector<std::reference_wrapper<IExpr>>& expressions) const {
    std::vector<std::reference_wrapper<IExpr>> dividend;
    std::vector<std::unique_ptr<IExpr>> divisor;
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
        divisor.emplace_back(math::exp(base.Copy(), math::negate(exp.Copy())));
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

    std::vector<std::reference_wrapper<IExpr>> coefPart;
    std::vector<std::reference_wrapper<IExpr>> varPart;

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