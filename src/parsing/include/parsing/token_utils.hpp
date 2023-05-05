#pragma once

#include <parsing/token.hpp>

namespace ezmath::parsing {
    namespace token {
        namespace bracket {
            namespace round {
                static constexpr auto opening = Token{Token::EType::Bracket, "("};
                static constexpr auto closing = Token{Token::EType::Bracket, ")"};
            }
            namespace box {
                static constexpr auto opening = Token{Token::EType::Bracket, "["};
                static constexpr auto closing = Token{Token::EType::Bracket, "]"};
            }
            namespace curly {
                static constexpr auto opening = Token{Token::EType::Bracket, "{"};
                static constexpr auto closing = Token{Token::EType::Bracket, "}"};
            }

            bool constexpr IsOpening(const Token token) { 
                return token == round::opening 
                    || token == box::opening
                    || token == curly::opening;
            }
            bool constexpr IsClosing(const Token token) {
                return token == round::closing 
                    || token == box::closing
                    || token == curly::closing;
            }
        }
        namespace operation {
            static constexpr auto plus = Token{Token::EType::Operator, "+"};
            static constexpr auto minus = Token{Token::EType::Operator, "-"};
            static constexpr auto pow = Token{Token::EType::Operator, "^"};
            static constexpr auto factorial = Token{Token::EType::Operator, "!"};
            static constexpr auto less = Token{Token::EType::Operator, "<"};
            static constexpr auto more = Token{Token::EType::Operator, ">"};
        }
        namespace command {
            static constexpr auto div = Token{Token::EType::Command, "\\div"};
            static constexpr auto cdot = Token{Token::EType::Command, "\\cdot"};
        }
        namespace matrix {
            namespace operation {
                static constexpr auto new_row = Token{Token::EType::Operator, "\\"};
                static constexpr auto new_col = Token{Token::EType::Operator, "&"};
            }
        }
    }
}

