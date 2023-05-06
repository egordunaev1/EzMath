#include <expression_tree/product.hpp>
#include <expression_tree/factory.hpp>
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
    if (subExpr->Is<Product>()) {
        for (auto& val : subExpr->As<Product>()->m_value) {
            Add(std::move(val));
        }
        return;
    }
    m_value.emplace_back(std::move(subExpr));
}

std::unique_ptr<IExpr> Product::Copy() const {
    std::vector<std::unique_ptr<IExpr>> newValue;
    newValue.reserve(m_value.size());
    for (const auto& val : m_value) {
        newValue.emplace_back(val->Copy());
    }
    return Factory::MakeProduct(std::move(newValue), Number{m_coefficient});
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