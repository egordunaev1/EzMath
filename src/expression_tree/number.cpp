#include <expression_tree/factory.hpp>
#include <expression_tree/exception.hpp>

namespace ezmath::expression_tree {

Number::Number(bigint val) 
    : m_value{std::move(val)}
{
    m_isConstant = true;
    if (m_value.Sign() == -1) {
        m_value *= bigint{-1};
        m_sign = -1;
    }
}

const Number::bigint& Number::Value() const noexcept { return m_value; }

std::unique_ptr<Expression> Number::Copy() const {
    return Factory::MakeNumber(m_value);
}

std::string Number::ToString() const {
    return m_value.ToString();
}

}