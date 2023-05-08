#include <expression_tree/factory.hpp>
#include <expression_tree/hash_utils.hpp>

namespace ezmath::expression_tree {

Symbol::Symbol(const std::string_view val) 
    : m_value{val} 
{}

void Symbol::SetName(const std::string_view s) {
    m_value = s;
    OnChange();
}

std::string_view Symbol::Name() const noexcept { return m_value; }

size_t Symbol::HashImpl() const {
    constexpr size_t RANDOM_BASE = 17690554549982570371u;
    return hash::combine(RANDOM_BASE, std::hash<std::string_view>()(m_value));
}

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