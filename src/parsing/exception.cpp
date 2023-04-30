#include <parsing/exception.hpp>
#include <fmt/format.h>

namespace ezmath::parsing::exception {
    
LexerException::LexerException(const std::string_view msg)
    : m_msg{fmt::format("Lexer error: {}", msg)}
{}

const char* LexerException::what() const noexcept { return m_msg.c_str(); }

} // namespace ezmath::parsing::exception
