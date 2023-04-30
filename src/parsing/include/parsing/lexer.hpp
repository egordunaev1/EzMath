#pragma once

#include <parsing/token.hpp>
#include <vector>

namespace ezmath::parsing {

class Lexer {
public:
    Lexer();
    
    Lexer(Lexer&&) = default;
    Lexer(const Lexer&) = delete;

    void Tokenize(std::string_view str);

    auto size() const {return m_tokens.size(); }
    auto begin() const { return m_tokens.begin(); }
    auto end() const { return m_tokens.end(); }
    auto at(const size_t ind) const { return m_tokens.at(ind); }
    auto operator[](const size_t ind) const { return m_tokens[ind]; }

    static constexpr std::string_view OPERATORS = "!+-^_&<>";
    static constexpr std::string_view BRACKETS = "{}[]()";

private:
    std::vector<Token> m_tokens;
    
    static constexpr size_t RESERVED_SIZE = 1000;
};

}