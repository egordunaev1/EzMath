#include <tree/factory.hpp>
#include <tree/hash_utils.hpp>
#include <unordered_set>
#include <ranges>

namespace ezmath::tree {

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

void Sum::Set(size_t index, std::unique_ptr<IExpr>&& value) {
    m_value[index] = std::move(value);
    OnChange();
}

void Sum::Erase(size_t index) {
    m_value.erase(m_value.begin() + index);
    OnChange();
}

void Sum::AddTerm(std::unique_ptr<IExpr>&& value) {
    Add(std::move(value));
    OnChange();
}

void Sum::Add(std::unique_ptr<Sum>&& other) {
    for (auto& val : other->m_value) {
        Add(std::move(val));
    }
    OnChange();
}

size_t Sum::Size() const noexcept {
    return m_value.size();
}

const IExpr& Sum::Get(size_t index) const noexcept {
    return *m_value.at(index);
}

const std::vector<std::unique_ptr<IExpr>>& Sum::Value() const noexcept {
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

    std::unordered_multiset<hash::Wrapper, hash::Hasher> lhs;
    std::unordered_multiset<hash::Wrapper, hash::Hasher> rhs;

    lhs.reserve(m_value.size());
    rhs.reserve(m_value.size());

    for (const auto& val : m_value)
        lhs.insert(val.get());
    for (const auto& val : otherAsSum->m_value)
        rhs.insert(val.get());

    return lhs == rhs;
}

size_t Sum::HashImpl() const {
    constexpr size_t RANDOM_BASE = 18251384670654659732u;   
    return hash::asymmetric_hash(RANDOM_BASE, m_value);
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