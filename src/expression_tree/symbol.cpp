#include <expression_tree/symbol.hpp>
#include <expression_tree/factory.hpp>

namespace ezmath::expression_tree {

Symbol::Symbol(const std::string_view val) : m_value{val} {}

std::unique_ptr<IExpr> Symbol::Copy() const {
    return Factory::MakeSymbol(m_value);
}

std::string Symbol::ToString() const {
    return std::string{m_value};
}

}