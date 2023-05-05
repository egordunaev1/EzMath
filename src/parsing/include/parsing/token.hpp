#pragma once

#include <string_view>

namespace ezmath::parsing {

struct Token {
    enum class EType {
        Symbol,
        Number,
        Command,
        Operator,
        Bracket
    };

    EType Type;
    std::string_view Value;

    constexpr bool operator==(const Token& other) const noexcept { return (Type == other.Type) && (Value == other.Value); }
};

}