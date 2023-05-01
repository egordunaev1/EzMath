#include <expression_tree/sum.hpp>
#include <ranges>

namespace ezmath::expression_tree {

Sum::Sum(std::vector<std::unique_ptr<IExpr>>&& subExpr)
    : m_value{std::make_move_iterator(subExpr.begin()), std::make_move_iterator(subExpr.end())}
{}

void Sum::Add(std::unique_ptr<IExpr>&& subExpr) {
    m_value.emplace_back(std::move(subExpr));
}

std::unique_ptr<IExpr> Sum::Copy() const {
    std::vector<std::unique_ptr<IExpr>> newValue;
    newValue.reserve(m_value.size());
    for (const auto& val : m_value) {
        newValue.emplace_back(val->Copy());
    }
    
    return std::make_unique<Sum>(std::move(newValue));
}

std::string Sum::ToString() const {
    std::string res;
    if (m_value.empty()) {
        return res;
    }

    res = m_value.front()->ToString();
    if (m_value.size() == 1) {
        return res;
    }

    res.reserve(256);
    for (auto it = ++m_value.begin(); it != m_value.end(); ++it) {
        res.push_back('+');
        res.append((*it)->ToString());
    }
    return res;
}

}