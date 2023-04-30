#include <expression_tree/product.hpp>
#include <ranges>

namespace ezmath::expression_tree {

Product::Product(std::vector<std::unique_ptr<IExpr>>&& subExpr, Number&& coefficient)
    : m_coefficient{std::move(coefficient)}
{
    for (auto& val : subExpr) {
        Add(std::move(val));
    }
}

void Product::Add(std::unique_ptr<IExpr>&& subExpr) {
    if (subExpr->GetTypeID() == Number::TypeID()) {
        const auto numVal = dynamic_cast<Number*>(subExpr.get());
        m_coefficient *= numVal->GetValue();
    } else {
        m_value.emplace_back(std::move(subExpr));
    }
}

size_t Product::TypeID() noexcept {
    return __COUNTER__;
}

size_t Product::GetTypeID() const noexcept {
    return TypeID();
}

std::unique_ptr<IExpr> Product::Copy() const {
    std::vector<std::unique_ptr<IExpr>> newValue;
    newValue.reserve(m_value.size());
    for (const auto& val : m_value) {
        newValue.emplace_back(val->Copy());
    }
    
    return std::make_unique<Product>(std::move(newValue));
}

std::string Product::ToString() const {
    std::string res;
    res.reserve(256);

    if (m_value.empty() || m_coefficient.GetValue() != 1) {
        res = m_coefficient.ToString();
    }

    if (m_value.empty()) {
        return res;
    }

    for (const auto& val : m_value) {
        res.append(val->ToString());
    }
    return res;
}

}