#include <tree/exception.hpp>
#include <fmt/format.h>

namespace ezmath::tree::exception {
    
CalcException::CalcException(const std::string_view msg)
    : m_msg{fmt::format("Calculation error: {}", msg)}
{}

const char* CalcException::what() const noexcept { return m_msg.c_str(); }

} // namespace ezmath::parsing::exception
