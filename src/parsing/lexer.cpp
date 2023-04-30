#include <fmt/format.h>
#include <parsing/lexer.hpp>
#include <parsing/exception.hpp>
#include <algorithm>
#include <ranges>

namespace ezmath::parsing {

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

ezmath::parsing::Lexer::Lexer() {
    m_tokens.reserve(RESERVED_SIZE);
}

void Lexer::Tokenize(const std::string_view str) {
    m_tokens.clear();

    using THandler = Token(*)(std::string_view);
    constexpr static std::array<THandler, 128> handlers = []() constexpr {
        constexpr auto isDigit = [](const char cc){ return '0' <= cc && cc <= '9'; };
        constexpr auto isAlpha = [](const char cc){ return ('a' <= cc && cc <= 'z') || ('A' <= cc && cc <= 'Z'); };

        std::array<THandler, 128> result{};
        for (char cc = 0; cc < 128; ++cc) {
            if (BRACKETS.contains(cc)) {
                result[cc] = [](std::string_view s){ 
                    return Token{Token::EType::Bracket, s.substr(0, 1)}; 
                };
            } else if (OPERATORS.contains(cc)) {
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

    for (const char* next = std::ranges::find_if_not(str, isspace); 
         next != str.end(); 
         next = std::ranges::find_if_not(next, str.end(), isspace)) 
    {
        const auto handler = handlers[*next];
        if (!handler) {
            throw exception::LexerException{fmt::format("Unexpected character: '{}' [{:#04x}]", *next, *next)};
        }
        
        const auto token = handler(str.substr(next - str.begin()));
        m_tokens.emplace_back(token);
        next += token.Value.size();
    }
}

}