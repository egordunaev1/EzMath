#include <expression_tree/factory.hpp>

namespace ezmath::expression_tree {

Negation::Negation(std::unique_ptr<Expression>&& val)
    : m_value{std::move(val)}
{}

const Expression& Negation::Value() const noexcept {
    return *m_value;
}

bool Negation::IsConstant() const {
    return m_value->IsConstant();
}

int Negation::Sign() const {
    return -m_value->Sign();
}

bool Negation::IsEqualTo(const Expression& other) const {
    return other.Is<Negation>() && other.As<Negation>()->m_value->IsEqualTo(*m_value);
}

std::unique_ptr<Expression> Negation::Copy() const {
    return Factory::Negate(m_value->Copy());
}

std::string Negation::ToString() const {
    auto res = m_value->ToString();
    if (m_value->Is<Sum>() || res.starts_with('-')) {
        return fmt::format("-({})", res);
    }
    return res;
}



}