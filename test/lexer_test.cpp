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

    Lexer m_lexer;

void ExpectSuccess(const TTest& test, const TAnsw& answ) {
    ASSERT_NO_THROW(m_lexer.Tokenize(test));
    ExpectEQ(answ);
}

void ExpectEQ(const TAnsw& answ) {
    EXPECT_EQ(std::distance(m_lexer.begin(), m_lexer.end()), std::distance(answ.begin(), answ.end())) << PrintTokens();

    auto TestAnsIt = answ.begin();
    auto LexerAnsIt = m_lexer.begin();
    while (TestAnsIt != answ.end() && LexerAnsIt != m_lexer.end()) {
        EXPECT_EQ(*TestAnsIt, *LexerAnsIt) 
            << fmt::format("TestAns: {}\nLexrAns: {}", ToString(*TestAnsIt), ToString(*LexerAnsIt));
        ++TestAnsIt;
        ++LexerAnsIt;
    }
}

std::string PrintTokens() {
    std::string res = "Tokens:\n";
    for (const auto token : m_lexer) {
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


TEST_F(LexerTest, TestNumber_correct) {
    TTest TEST = "1234567890 1234.567890";
    TAnsw ANSW = {
        Token{Token::EType::Number, "1234567890"},
        Token{Token::EType::Number, "1234.567890"}
    };
    ExpectSuccess(TEST, ANSW);
}

TEST_F(LexerTest, TestNumber_invalid) {
    TTest TEST1 = "1234567890.";
    TTest TEST2 = "123.4567.890";
    TTest TEST3 = ".1234567890";
    EXPECT_THROW(m_lexer.Tokenize(TEST1), ezmath::parsing::exception::LexerException);
    EXPECT_THROW(m_lexer.Tokenize(TEST2), ezmath::parsing::exception::LexerException);
    EXPECT_THROW(m_lexer.Tokenize(TEST3), ezmath::parsing::exception::LexerException);
}


TEST_F(LexerTest, TestCommand_correct) {
    TTest TEST = "\\command \\CoMmAnD \\cOmMaNd";
    TAnsw ANSW = {
        Token{Token::EType::Command, "\\command"},
        Token{Token::EType::Command, "\\CoMmAnD"},
        Token{Token::EType::Command, "\\cOmMaNd"},
    };
    ExpectSuccess(TEST, ANSW);

    for (char cc = 1; cc < 128; ++cc) {
        if (std::isalpha(cc)) {
            const std::string TEST = std::string{"\\"} + cc;
            TAnsw ANSW = {
                Token{Token::EType::Command, TEST}
            };
            ExpectSuccess(TEST, ANSW);
        }
    }
}

TEST_F(LexerTest, TestCommand_invalid) {
    for (char cc = 1; cc < 128; ++cc) {
        if (!std::isalpha(cc) && cc != '\\') {
            const std::string TEST = std::string{"\\"} + cc;
            EXPECT_THROW(m_lexer.Tokenize(TEST) , ezmath::parsing::exception::LexerException) 
                << fmt::format("Test: \"{}\":\n{}", TEST, PrintTokens());
        }
    }
}

TEST_F(LexerTest, TestSymbol) {
    for (char cc = 1; cc < 128; ++cc) {
        const std::string TEST = std::string{} + cc;
        if (std::isalpha(cc)) {
            TAnsw ANSW = {
                Token{Token::EType::Symbol, TEST}
            };
            ExpectSuccess(TEST, ANSW);
        } else {
            try {
                m_lexer.Tokenize(TEST);
                EXPECT_NE(m_lexer.at(0).Type, Token::EType::Symbol) << fmt::format("Test: \"{}\":\n{}", TEST, PrintTokens());
            } catch (...) {}
        }
    }
}

TEST_F(LexerTest, TestBracket) {
    for (char cc = 1; cc < 128; ++cc) {
        const std::string TEST = std::string{} + cc;
        if (Lexer::BRACKETS.contains(cc)) {
            TAnsw ANSW = {
                Token{Token::EType::Bracket, TEST}
            };
            ExpectSuccess(TEST, ANSW);
        } else {
            try {
                m_lexer.Tokenize(TEST);
                EXPECT_NE(m_lexer.at(0).Type, Token::EType::Bracket) << fmt::format("Test: \"{}\":\n{}", TEST, PrintTokens());
            } catch (...) {}
        }
    }
}

TEST_F(LexerTest, TestOperator) {
    for (char cc = 1; cc < 128; ++cc) {
        const std::string TEST = std::string{} + cc;
        if (Lexer::OPERATORS.contains(cc)) {
            TAnsw ANSW = {
                Token{Token::EType::Operator, TEST}
            };
            ExpectSuccess(TEST, ANSW);
        } else {
            try {
                m_lexer.Tokenize(TEST);
                EXPECT_NE(m_lexer.at(0).Type, Token::EType::Operator) << fmt::format("Test: \"{}\":\n{}", TEST, PrintTokens());
            } catch (...) {}
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