#include <expression_tree/factory.hpp>

namespace ezmath::expression_tree {

Symbol::Symbol(const std::string_view val) 
    : m_value{val} 
{}

std::string_view Symbol::Name() const noexcept { return m_value; }

bool Symbol::IsEqualTo(const IExpr& other) const {
    return other.Is<Symbol>() && (m_value == other.As<Symbol>()->m_value);
}

std::unique_ptr<IExpr> Symbol::Copy() const {
    return math::symbol(m_value);
}

std::string Symbol::ToString() const {
    return std::string{m_value};
}

}