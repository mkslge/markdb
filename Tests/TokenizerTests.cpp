//
// Created by Mark on 4/12/26.
//

#include <gtest/gtest.h>
#include "../SyntaxLayer/Tokenizer.h"

namespace {
std::vector<TokenType> getTypes(const std::vector<Token>& tokens) {
    std::vector<TokenType> types;
    types.reserve(tokens.size());

    for (const Token& token : tokens) {
        types.push_back(token.getType());
    }

    return types;
}
}

TEST(TokenizerTests, TokenizeSelectStarFromTableStatement) {
    Tokenizer tokenizer;

    std::optional<std::vector<Token>> tokens = tokenizer.tokenize("SELECT * FROM users;");

    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(getTypes(tokens.value()), (std::vector<TokenType>{
        TokenType::SELECT,
        TokenType::STAR,
        TokenType::FROM,
        TokenType::IDENTIFIER,
        TokenType::SEMICOLON
    }));
    EXPECT_EQ(tokens->at(3).getLexeme(), "users");
}

TEST(TokenizerTests, TokenizeCommaSeparatedProjectionList) {
    Tokenizer tokenizer;

    std::optional<std::vector<Token>> tokens = tokenizer.tokenize("SELECT id, name FROM users;");

    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(getTypes(tokens.value()), (std::vector<TokenType>{
        TokenType::SELECT,
        TokenType::IDENTIFIER,
        TokenType::COMMA,
        TokenType::IDENTIFIER,
        TokenType::FROM,
        TokenType::IDENTIFIER,
        TokenType::SEMICOLON
    }));
    EXPECT_EQ(tokens->at(1).getLexeme(), "id");
    EXPECT_EQ(tokens->at(3).getLexeme(), "name");
}

TEST(TokenizerTests, TokenizeKeywordsCaseInsensitivelyButPreserveIdentifierLexemes) {
    Tokenizer tokenizer;

    std::optional<std::vector<Token>> tokens = tokenizer.tokenize("select Name from Users;");

    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(tokens->at(0).getType(), TokenType::SELECT);
    EXPECT_EQ(tokens->at(1).getType(), TokenType::IDENTIFIER);
    EXPECT_EQ(tokens->at(1).getLexeme(), "Name");
    EXPECT_EQ(tokens->at(2).getType(), TokenType::FROM);
    EXPECT_EQ(tokens->at(3).getLexeme(), "Users");
}

TEST(TokenizerTests, TokenizeAcceptsUnderscoresAndDigitsInsideIdentifiers) {
    Tokenizer tokenizer;

    std::optional<std::vector<Token>> tokens = tokenizer.tokenize("SELECT user_1, account2 FROM app_users;");

    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(tokens->at(1).getLexeme(), "user_1");
    EXPECT_EQ(tokens->at(3).getLexeme(), "account2");
    EXPECT_EQ(tokens->at(5).getLexeme(), "app_users");
}

TEST(TokenizerTests, TokenizeHandlesWhitespaceAndNewlines) {
    Tokenizer tokenizer;

    std::optional<std::vector<Token>> tokens = tokenizer.tokenize(" \nSELECT\tid,\nname FROM\tusers ; ");

    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(getTypes(tokens.value()), (std::vector<TokenType>{
        TokenType::SELECT,
        TokenType::IDENTIFIER,
        TokenType::COMMA,
        TokenType::IDENTIFIER,
        TokenType::FROM,
        TokenType::IDENTIFIER,
        TokenType::SEMICOLON
    }));
}

TEST(TokenizerTests, TokenizeEmptyInputReturnsEmptyTokenStream) {
    Tokenizer tokenizer;

    std::optional<std::vector<Token>> tokens = tokenizer.tokenize("   \n\t  ");

    ASSERT_TRUE(tokens.has_value());
    EXPECT_TRUE(tokens->empty());
}

TEST(TokenizerTests, TokenizeRejectsUnknownSymbols) {
    Tokenizer tokenizer;

    EXPECT_FALSE(tokenizer.tokenize("SELECT @ FROM users;").has_value());
    EXPECT_FALSE(tokenizer.tokenize("SELECT name FROM users!;").has_value());
    EXPECT_FALSE(tokenizer.tokenize("INSERT INTO users VALUES [1];").has_value());
}

TEST(TokenizerTests, TokenizeSupportsIntegerLiteralsForInsertValues) {
    Tokenizer tokenizer;

    std::optional<std::vector<Token>> tokens = tokenizer.tokenize("INSERT INTO users VALUES (1, 23);");

    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(tokens->at(0).getType(), TokenType::INSERT);
    EXPECT_EQ(tokens->at(1).getType(), TokenType::INTO);
    EXPECT_EQ(tokens->at(3).getType(), TokenType::VALUES);
    EXPECT_EQ(tokens->at(4).getType(), TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens->at(5).getType(), TokenType::INTEGER_LITERAL);
    EXPECT_EQ(tokens->at(5).getLexeme(), "1");
    EXPECT_EQ(tokens->at(7).getType(), TokenType::INTEGER_LITERAL);
    EXPECT_EQ(tokens->at(7).getLexeme(), "23");
    EXPECT_EQ(tokens->at(8).getType(), TokenType::RIGHT_PAREN);
}

TEST(TokenizerTests, TokenizeRejectsIdentifiersStartingWithDash) {
    Tokenizer tokenizer;

    EXPECT_FALSE(tokenizer.tokenize("SELECT user-name FROM accounts;").has_value());
}

TEST(TokenizerTests, TokenizeTreatsKeywordPrefixInsideIdentifierAsIdentifier) {
    Tokenizer tokenizer;

    std::optional<std::vector<Token>> tokens = tokenizer.tokenize("SELECT select_list FROM from_table;");

    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(tokens->at(1).getType(), TokenType::IDENTIFIER);
    EXPECT_EQ(tokens->at(1).getLexeme(), "select_list");
    EXPECT_EQ(tokens->at(3).getType(), TokenType::IDENTIFIER);
    EXPECT_EQ(tokens->at(3).getLexeme(), "from_table");
}

TEST(TokenizerTests, TokenizeInsertStringLiteralsWithoutQuotesInLexeme) {
    Tokenizer tokenizer;

    std::optional<std::vector<Token>> tokens =
            tokenizer.tokenize("INSERT INTO users VALUES ('mark', 'ada@example.com');");

    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(tokens->at(5).getType(), TokenType::STRING_LITERAL);
    EXPECT_EQ(tokens->at(5).getLexeme(), "mark");
    EXPECT_EQ(tokens->at(7).getType(), TokenType::STRING_LITERAL);
    EXPECT_EQ(tokens->at(7).getLexeme(), "ada@example.com");
}

TEST(TokenizerTests, TokenizeRejectsUnterminatedStringLiteral) {
    Tokenizer tokenizer;

    EXPECT_FALSE(tokenizer.tokenize("INSERT INTO users VALUES ('mark);").has_value());
}
