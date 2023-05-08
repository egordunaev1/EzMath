#include <expression_tree/factory.hpp>
#include <ranges>

namespace ezmath::expression_tree {

Sum::Sum(std::vector<std::unique_ptr<IExpr>>&& values) {
    for (auto& val : values) {
        Add(std::move(val));
    }
}

void Sum::Add(std::unique_ptr<IExpr>&& subExpr) {
    if (subExpr->Is<Sum>()) {
        for (auto& val : subExpr->As<Sum>()->m_value) {
            Add(std::move(val));
        }
        return;
    }
    m_value.emplace_back(std::move(subExpr));
}

const std::list<std::unique_ptr<IExpr>>& Sum::Value() const noexcept {
    return m_value;
}

bool Sum::IsConstant() const {
    return std::all_of(m_value.begin(), m_value.end(), 
        [](const auto& val){ return val->IsConstant(); });
}

int Sum::Sign() const {
    const size_t neg = std::ranges::count_if(m_value, [](const auto& val){return val->Sign() == -1;});
    return (neg == m_value.size()) ? -1 : 1;
}

bool Sum::IsEqualTo(const IExpr& other) const {
    if (!other.Is<Sum>()) {
        return false;
    }
    const auto otherAsSum = other.As<Sum>();
    if (otherAsSum->m_value.size() != m_value.size()) {
        return false;
    }

    bool isMatched[m_value.size()];
    std::memset(&isMatched, 0, m_value.size());
    for (const auto& val : m_value) {
        size_t index = 0;
        for (const auto& otherVal : otherAsSum->m_value) {
            if (!isMatched[index] && val->IsEqualTo(*otherVal)) {
                isMatched[index] = true;
                break;
            }
            ++index;
        }
        if (const auto found = (index == otherAsSum->m_value.size()); found) {
            return false;
        }
    }
    return true;
}

std::unique_ptr<IExpr> Sum::Copy() const {
    constexpr auto copy = [](const auto& val) { return val->Copy(); };

    std::vector<std::unique_ptr<IExpr>> values;
    std::transform(m_value.begin(), m_value.end(), std::back_inserter(values), copy);
    return math::add(std::move(values));
}

std::string Sum::ToString() const {
    std::string res;
    if (m_value.empty()) {
        return "0";
    }

    res.reserve(256);
    res.append(m_value.front()->ToString());

    if (m_value.size() == 1) {
        return res;
    }

    for (auto it = ++m_value.begin(); it != m_value.end(); ++it) {
        if (it->get()->Sign() == 1) {
            res.push_back('+');
        }
        res.append(it->get()->ToString());
    }
    return res;
}

}