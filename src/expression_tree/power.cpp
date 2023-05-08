#include <expression_tree/factory.hpp>
#include <expression_tree/hash_utils.hpp>
#include <fmt/format.h>

namespace ezmath::expression_tree {

Power::Power(std::unique_ptr<IExpr>&& base, std::unique_ptr<IExpr>&& exp)
    : m_base{std::move(base)}
    , m_exp{std::move(exp)}
{}

const IExpr& Power::Base() const noexcept { return *m_base; }

const IExpr& Power::Exp() const noexcept { return *m_exp; }

size_t Power::Hash() const {
    constexpr size_t RANDOM_BASE = 17690554549982570371u;
    if (m_bufferedHash) {
        return m_bufferedHash;
    }
    return m_bufferedHash = hash::combine(RANDOM_BASE, m_base->Hash(), m_exp->Hash());
}

bool Power::IsConstant() const {
    return m_base->IsConstant() && m_exp->IsConstant();
}

bool Power::IsEqualTo(const IExpr& other) const {
    if (Hash() != other.Hash()) {
        return false;
    }
    return other.Is<Power>() 
        && m_base->IsEqualTo(*other.As<Power>()->m_base)
        && m_exp->IsEqualTo(*other.As<Power>()->m_exp);
}

std::unique_ptr<IExpr> Power::Copy() const {
    return math::exp(m_base->Copy(), m_exp->Copy());
}

std::string Power::ToString() const {
    if (m_base->Is<Product>() || m_base->Is<Sum>()) {
        return fmt::format("({})^{{{}}}", m_base->ToString(), m_exp->ToString());
    }
    return fmt::format("{}^{{{}}}", m_base->ToString(), m_exp->ToString());
}

}