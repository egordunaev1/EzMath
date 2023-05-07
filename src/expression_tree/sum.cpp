#include <expression_tree/factory.hpp>

namespace ezmath::expression_tree {

Sum::Sum() {
    m_sign = -1;
}


void Sum::Add(std::unique_ptr<Expression>&& subExpr) {
    if (subExpr->Is<Sum>()) {
        for (auto& val : subExpr->As<Sum>()->m_value) {
            Add(std::move(val));
        }
        return;
    }

    if (m_sign == -1) {
        // if at least one positive term, sign is +
        if (subExpr->Sign() == 1) {
            m_sign = 1;
            for (const auto& val : m_value) {
                val->ToggleSign();
            }
        } else {
            subExpr->ToggleSign();
        }
    }

    m_isConstant &= subExpr->IsConstant();
    m_value.emplace_back(std::move(subExpr));
}

std::unique_ptr<Expression> Sum::Copy() const {
    auto sum = Factory::MakeSum();
    for (const auto& val : m_value) {
        sum->Add(val->Copy());
    }
    return sum;
}

void Sum::ToString(std::string& res, Expression& expr) const {
    if (expr.Sign() * m_sign == 1 && !res.empty()) {
        res.push_back('+');
    }

    if (m_sign == 1) {
        res.append(expr.ToString());
        return;
    }

    expr.ToggleSign();
    res.append(expr.ToString());
    expr.ToggleSign();
}

std::string Sum::ToString() const {
    std::string res;
    if (m_value.empty()) {
        return res;
    }

    res.reserve(256);
    ToString(res, *m_value.front());
    if (m_value.size() == 1) {
        return res;
    }

    for (auto it = ++m_value.begin(); it != m_value.end(); ++it) {
        auto val = (*it)->ToString();
        if (!val.starts_with('-')) {
            res.push_back('+');
        }
        res.append(std::move(val));
    }
    return res;
}

}