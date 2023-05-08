#include <expression_tree/factory.hpp>
#include <fmt/format.h>

namespace ezmath::expression_tree {

Power::Power(std::unique_ptr<Expression>&& base, std::unique_ptr<Expression>&& exp)
    : m_base{std::move(base)}
    , m_exp{std::move(exp)}
{}

const Expression& Power::Base() const noexcept { return *m_base; }

const Expression& Power::Exp() const noexcept { return *m_exp; }

bool Power::IsConstant() const {
    return m_base->IsConstant() && m_exp->IsConstant();
}

bool Power::IsEqualTo(const Expression& other) const {
    return other.Is<Power>() 
        && m_base->IsEqualTo(*other.As<Power>()->m_base)
        && m_exp->IsEqualTo(*other.As<Power>()->m_exp);
}

std::unique_ptr<Expression> Power::Copy() const {
    return math::exp(m_base->Copy(), m_exp->Copy());
}

std::string Power::ToString() const {
    if (m_base->Is<Product>() || m_base->Is<Sum>()) {
        return fmt::format("({})^{{{}}}", m_base->ToString(), m_exp->ToString());
    }
    return fmt::format("{}^{{{}}}", m_base->ToString(), m_exp->ToString());
}

}