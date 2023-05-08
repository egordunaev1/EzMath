#include <expression_tree/factory.hpp>

namespace ezmath::expression_tree {

Symbol::Symbol(const std::string_view val) 
    : m_value{val} 
{
    m_sign = 1;
    m_isConstant = false;
}

std::string_view Symbol::Name() const noexcept { return m_value; }

bool Symbol::IsEqualTo(const Expression& other) const {
    return other.Is<Symbol>() && (m_value == other.As<Symbol>()->m_value);
}

std::unique_ptr<Expression> Symbol::Copy() const {
    return Factory::MakeSymbol(m_value);
}

std::string Symbol::ToString() const {
    return std::string{m_value};
}

}