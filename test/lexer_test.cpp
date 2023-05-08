#include <gtest/gtest.h>
#include <parsing/exception.hpp>
#include <parsing/lexer.hpp>
#include <fmt/format.h>
#include <ranges>

namespace ezmath::test {

using namespace parsing;

class LexerTest : public ::testing::Test {
protected:
    using TTest = std::string_view;
    using TAnsw = std::vector<Token>;

TAnsw Tokenize(const TTest& test) {
    Lexer lexer{test};
    TAnsw res;
    while (const auto token = lexer.GetToken()) {
        res.push_back(*token);
        lexer.NextToken();
    }
    return res;
}

void ExpectSuccess(const TTest& test, const TAnsw& answ) {
    TAnsw lexerAnsw;
    ASSERT_NO_THROW(lexerAnsw = Tokenize(test));
    ExpectEQ(answ, lexerAnsw);
}

void ExpectEQ(const TAnsw& testAnsw, const TAnsw& lexerAnsw) {
    EXPECT_EQ(testAnsw.size(), lexerAnsw.size()) << PrintTokens(lexerAnsw);

    auto TestAnsIt = testAnsw.begin();
    auto LexerAnsIt = lexerAnsw.begin();
    while (TestAnsIt != testAnsw.end() && LexerAnsIt != lexerAnsw.end()) {
        EXPECT_EQ(*TestAnsIt, *LexerAnsIt) 
            << fmt::format("TestAns: {}\nLexrAns: {}", ToString(*TestAnsIt), ToString(*LexerAnsIt));
        ++TestAnsIt;
        ++LexerAnsIt;
    }
}

std::string PrintTokens(const TAnsw& lexerAnsw) {
    std::string res = "Tokens:\n";
    for (const auto token : lexerAnsw) {
        res.append('>' + ToString(token) + '\n');
    }
    return res;
}

private:
    std::string ToString(Token token) {
        std::string res;
        switch (token.Type) {
            case Token::EType::Symbol: res = fmt::format("Symbol(\"{}\")", token.Value); break;
            case Token::EType::Number: res = fmt::format("Number(\"{}\")", token.Value); break;
            case Token::EType::Command: res = fmt::format("Command(\"{}\")", token.Value); break;
            case Token::EType::Operator: res = fmt::format("Operator(\"{}\")", token.Value); break;
            case Token::EType::Bracket: res = fmt::format("Bracket(\"{}\")", token.Value); break;
        }
        return res;
    };
};


TEST_F(LexerTest, TestNumber) {
    TAnsw tmp;
    TTest TEST = "1234567890 1234.567890";
    TAnsw ANSW = {
        Token{Token::EType::Number, "1234567890"},
        Token{Token::EType::Number, "1234.567890"}
    };
    ExpectSuccess(TEST, ANSW);

    TTest TEST1 = "1234567890.";
    TTest TEST2 = "123.4567.890";
    TTest TEST3 = ".1234567890";
    EXPECT_THROW(tmp = Tokenize(TEST1), ezmath::parsing::exception::LexerException) << PrintTokens(tmp);
    EXPECT_THROW(tmp = Tokenize(TEST2), ezmath::parsing::exception::LexerException) << PrintTokens(tmp);
    EXPECT_THROW(tmp = Tokenize(TEST3), ezmath::parsing::exception::LexerException) << PrintTokens(tmp);
}

TEST_F(LexerTest, TestCommand) {
    TAnsw tmp;
    TTest TEST = "\\command \\CoMmAnD \\cOmMaNd";
    TAnsw ANSW = {
        Token{Token::EType::Command, "\\command"},
        Token{Token::EType::Command, "\\CoMmAnD"},
        Token{Token::EType::Command, "\\cOmMaNd"},
    };
    ExpectSuccess(TEST, ANSW);

    for (char cc = 1; cc < std::numeric_limits<char>::max(); ++cc) {
        if (cc == '\\') continue;
        if (std::isalpha(cc)) {
            const std::string TEST = std::string{"\\"} + cc;
            TAnsw ANSW = {
                Token{Token::EType::Command, TEST}
            };
            ExpectSuccess(TEST, ANSW);
        } 
        else {
            const std::string TEST = std::string{"\\"} + cc;
            EXPECT_THROW(tmp = Tokenize(TEST) , ezmath::parsing::exception::LexerException) 
                << fmt::format("Test: \"{}\":\n{}", TEST, PrintTokens(tmp));
        }
    }
}

TEST_F(LexerTest, TestSymbol) {
    TAnsw tmp;
    for (char cc = 1; cc < std::numeric_limits<char>::max(); ++cc) {
        const std::string TEST = std::string{} + cc;
        if (std::isalpha(cc)) {
            TAnsw ANSW = {
                Token{Token::EType::Symbol, TEST}
            };
            ExpectSuccess(TEST, ANSW);
        } else {
            try {
                tmp = Tokenize(TEST);
                if (tmp.empty()) {
                    continue;
                }
                EXPECT_NE(tmp.front().Type, Token::EType::Symbol) 
                    << fmt::format("Test: \"{}\":\n{}", TEST, PrintTokens(tmp));
            } catch (const parsing::exception::LexerException&) {}
        }
    }
}

TEST_F(LexerTest, TestBracket) {
    TAnsw tmp;
    for (char cc = 1; cc < std::numeric_limits<char>::max(); ++cc) {
        const std::string TEST = std::string{} + cc;
        if (Lexer::BRACKETS.contains(cc)) {
            TAnsw ANSW = {
                Token{Token::EType::Bracket, TEST}
            };
            ExpectSuccess(TEST, ANSW);
        } else {
            try {
                tmp = Tokenize(TEST);
                if (tmp.empty()) {
                    continue;
                }
                EXPECT_NE(tmp.front().Type, Token::EType::Bracket) 
                    << fmt::format("Test: \"{}\":\n{}", TEST, PrintTokens(tmp));
            } catch (const parsing::exception::LexerException&) {}
        }
    }
}

TEST_F(LexerTest, TestOperator) {
    TAnsw tmp;
    for (char cc = 1; cc < std::numeric_limits<char>::max(); ++cc) {
        const std::string TEST = std::string{} + cc;
        if (Lexer::OPERATORS.contains(cc)) {
            TAnsw ANSW = {
                Token{Token::EType::Operator, TEST}
            };
            ExpectSuccess(TEST, ANSW);
        } else {
            try {
                tmp = Tokenize(TEST);
                if (tmp.empty()) {
                    continue;
                }
                EXPECT_NE(tmp.front().Type, Token::EType::Operator) 
                    << fmt::format("Test: \"{}\":\n{}", TEST, PrintTokens(tmp));
            } catch (const parsing::exception::LexerException&) {}
        }
    }
}

TEST_F(LexerTest, MixedTest) {
    TTest TEST = R"(\matrix{
                        a&&&0 \\
                        &b&&  \\
                        &&c&  \\
                        0&&&0.0
                    })";
    TAnsw ANSW = {
        Token{Token::EType::Command, "\\matrix"},
        Token{Token::EType::Bracket, "{"},

        Token{Token::EType::Symbol, "a"},
        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Number, "0"},
        Token{Token::EType::Operator, "\\\\"},

        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Symbol, "b"},
        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Operator, "\\\\"},

        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Symbol, "c"},
        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Operator, "\\\\"},

        Token{Token::EType::Number, "0"},
        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Operator, "&"},
        Token{Token::EType::Number, "0.0"},

        Token{Token::EType::Bracket, "}"},
    };

    ExpectSuccess(TEST, ANSW);
}

}