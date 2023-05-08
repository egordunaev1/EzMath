#include <expression_tree/factory.hpp>
#include <expression_tree/hash_utils.hpp>

namespace ezmath::expression_tree {

Negation::Negation(std::unique_ptr<IExpr>&& val)
    : m_value{std::move(val)}
{}

void Negation::Set(std::unique_ptr<IExpr>&& val) {
    m_value = std::move(val);
    OnChange();
}

const IExpr& Negation::Value() const noexcept {
    return *m_value;
}

bool Negation::IsConstant() const {
    return m_value->IsConstant();
}

size_t Negation::HashImpl() const {
    constexpr size_t RANDOM_BASE = 18406927461344389294u;
    return hash::combine(RANDOM_BASE, m_value->Hash());
}

int Negation::Sign() const {
    return -m_value->Sign();
}

bool Negation::IsEqualTo(const IExpr& other) const {
    if (Hash() != other.Hash()) {
        return false;
    }
    return other.Is<Negation>() && other.As<Negation>()->m_value->IsEqualTo(*m_value);
}

std::unique_ptr<IExpr> Negation::Copy() const {
    return math::negate(m_value->Copy());
}

std::string Negation::ToString() const {
    auto res = m_value->ToString();
    if (m_value->Is<Sum>() || res.starts_with('-')) {
        return fmt::format("-({})", res);
    }
    return res;
}

}