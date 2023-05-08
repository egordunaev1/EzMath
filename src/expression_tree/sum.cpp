#include <expression_tree/factory.hpp>
#include <expression_tree/hash_utils.hpp>
#include <unordered_set>
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

struct Wrapper {
    Wrapper() {}
    Wrapper(IExpr* val) : Value(val) {}

    IExpr* Value;

    friend bool operator==(const Wrapper& lhs, const Wrapper& rhs) { return lhs.Value->IsEqualTo(*rhs.Value); }
};

struct Hasher {
    size_t operator()(const Wrapper& wrapper) const { return wrapper.Value->Hash(); }
};

bool Sum::IsEqualTo(const IExpr& other) const {
    if (Hash() != other.Hash()) {
        return false;
    }
    if (!other.Is<Sum>()) {
        return false;
    }

    const auto otherAsSum = other.As<Sum>();
    if (otherAsSum->m_value.size() != m_value.size()) {
        return false;
    }

    std::unordered_multiset<Wrapper, Hasher> lhs;
    std::unordered_multiset<Wrapper, Hasher> rhs;

    lhs.reserve(m_value.size());
    rhs.reserve(m_value.size());

    for (const auto& val : m_value)
        lhs.insert(val.get());
    for (const auto& val : otherAsSum->m_value)
        rhs.insert(val.get());

    return lhs == rhs;
}

size_t Sum::Hash() const {
    constexpr size_t RANDOM_BASE = 18251384670654659732u;

    if (m_bufferedHash) {
        return m_bufferedHash;
    }
    
    size_t result = RANDOM_BASE;
    for (const auto& val : m_value) {
        hash::combine(result, val->Hash());
    }
    return m_bufferedHash = result;
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