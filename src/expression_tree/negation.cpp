#include <expression_tree/factory.hpp>

namespace ezmath::expression_tree {

Negation::Negation(std::unique_ptr<IExpr>&& val)
    : m_value{std::move(val)}
{}

const IExpr& Negation::Value() const noexcept {
    return *m_value;
}

bool Negation::IsConstant() const {
    return m_value->IsConstant();
}

int Negation::Sign() const {
    return -m_value->Sign();
}

bool Negation::IsEqualTo(const IExpr& other) const {
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