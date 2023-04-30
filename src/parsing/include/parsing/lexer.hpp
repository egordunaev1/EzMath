#pragma once

#include <parsing/token.hpp>
#include <vector>
#include <optional>

namespace ezmath::parsing {

class Lexer {
public:
    Lexer(std::string_view text);
    Lexer(Lexer&&) = default;
    Lexer(const Lexer&) = delete;

    std::optional<Token> GetToken();
    bool NextToken();
    const char* GetChar();
    void NextChar();

    static constexpr std::string_view OPERATORS = "!+-^_&<>";
    static constexpr std::string_view BRACKETS = "{}[]()";

private:
    std::string_view m_text;
    const char* m_iterator;
    Token m_bufferedToken;
};

}