#include <fmt/format.h>
#include <parsing/lexer.hpp>
#include <parsing/exception.hpp>
#include <algorithm>
#include <ranges>

namespace ezmath::parsing {

using THandler = Token(*)(std::string_view);

Token ReadNumber(const std::string_view str) {
    constexpr auto isnumdot = [](const char cc){ return std::isdigit(cc) || cc == '.'; };
    constexpr auto isdot = [](const char cc){ return cc == '.'; };

    const auto end = std::ranges::find_if_not(str, isnumdot);
    const auto token = str.substr(0, end - str.begin());

    if (token.ends_with('.')) {
        throw exception::LexerException{"number ends with '.'"};
    }

    if (std::ranges::count_if(str, isdot) > 1) {
        throw exception::LexerException{"number contains more than 1 dot"};
    }

    return {Token::EType::Number, token};
}

Token ReadCommand(const std::string_view str) {
    const auto end = std::ranges::find_if_not(str.substr(1), isalpha);
    const auto token = str.substr(0, end - str.begin());

    if (token.size() == 1) {
        throw exception::LexerException{"expected command name after '/'"};
    }
    
    return {
        .Type=Token::EType::Command,
        .Value=token
    };
}

Token Read2SOrCommand(const std::string_view str) {
    return str.starts_with("\\\\") 
        ? Token{Token::EType::Operator, str.substr(0, 2)}
        : ReadCommand(str);
}

constexpr static std::array<THandler, std::numeric_limits<char>::max() + 1> handlers = []() constexpr {
    constexpr auto isDigit = [](const char cc){ return '0' <= cc && cc <= '9'; };
    constexpr auto isAlpha = [](const char cc){ return ('a' <= cc && cc <= 'z') || ('A' <= cc && cc <= 'Z'); };

    std::array<THandler, std::numeric_limits<char>::max() + 1> result{};
    for (char cc = 0; cc < std::numeric_limits<char>::max(); ++cc) {
        if (Lexer::BRACKETS.contains(cc)) {
            result[cc] = [](std::string_view s){ 
                return Token{Token::EType::Bracket, s.substr(0, 1)}; 
            };
        } else if (Lexer::OPERATORS.contains(cc)) {
            result[cc] = [](std::string_view s){ 
                return Token{Token::EType::Operator, s.substr(0, 1)};
            };
        } else if (isAlpha(cc)) {
            result[cc] = [](std::string_view s){ 
                return Token{Token::EType::Symbol, s.substr(0, 1)};
            };
        } else if (isDigit(cc)) {
            result[cc] = ReadNumber;
        } else {
            result[cc] = nullptr;
        }
    }
    result['\\'] = Read2SOrCommand;
    return result;
}();

ezmath::parsing::Lexer::Lexer(const std::string_view text) 
    : m_text{text}
    , m_iterator{text.begin()}
{}

std::optional<Token> Lexer::GetToken() {
    if (m_bufferedToken.Value.begin() == m_iterator) {
        return m_bufferedToken;
    }

    m_iterator = std::ranges::find_if_not(m_iterator, m_text.end(), isspace);
    if (m_iterator == m_text.end()) {
        return std::nullopt;
    }

    const auto handler = handlers[*m_iterator];
    if (!handler) {
        throw exception::LexerException{fmt::format("Unexpected character: '{}' [{:#04x}]", *m_iterator, *m_iterator)};
    }
    
    return m_bufferedToken = handler({m_iterator, m_text.end()});
}

bool Lexer::NextToken() {
    if (const auto curToken = GetToken()) {
        m_iterator = curToken->Value.end();
        return true;
    }
    return false;
}

const char* Lexer::GetChar() {
    m_iterator = std::ranges::find_if_not(m_iterator, m_text.end(), isspace);
    if (m_iterator == m_text.end()) {
        return nullptr;
    }
    return m_iterator;
}

void Lexer::NextChar() {
    if (m_iterator == m_text.end()) {
        throw exception::LexerException{"Iterator out of bounds"};
    }
    ++m_iterator;
}

}