//
// Created by Mark on 4/12/26.
//

#include <gtest/gtest.h>
#include "../SyntaxLayer/InsertParseResult.h"
#include "../SyntaxLayer/Parser.h"
#include "../SyntaxLayer/Tokenizer.h"

namespace {
ParseResult parseSql(const std::string& sql) {
    Tokenizer tokenizer;
    Parser parser;

    std::optional<std::vector<Token>> tokens = tokenizer.tokenize(sql);
    if (!tokens.has_value()) {
        return ParseResult::failure("Tokenizer failed");
    }

    return parser.parseSelect(tokens.value());
}

InsertParseResult parseInsertSql(const std::string& sql) {
    Tokenizer tokenizer;
    Parser parser;

    std::optional<std::vector<Token>> tokens = tokenizer.tokenize(sql);
    if (!tokens.has_value()) {
        return InsertParseResult::failure("Tokenizer failed");
    }

    return parser.parseInsert(tokens.value());
}
}

TEST(ParserTests, ParseSingleColumnSelectStatement) {
    ParseResult result = parseSql("SELECT id FROM users;");

    ASSERT_TRUE(result.succeeded()) << result.getErrorMessage();
    ASSERT_TRUE(result.getStatement().has_value());
    EXPECT_EQ(result.getStatement()->getTableName(), "users");
    ASSERT_EQ(result.getStatement()->getColumns().size(), 1);
    EXPECT_EQ(result.getStatement()->getColumns()[0].getColumnName(), "id");
    EXPECT_FALSE(result.getStatement()->isWildcardSelect());
}

TEST(ParserTests, ParseMultiColumnSelectStatement) {
    ParseResult result = parseSql("SELECT id, name FROM users;");

    ASSERT_TRUE(result.succeeded()) << result.getErrorMessage();
    ASSERT_EQ(result.getStatement()->getColumns().size(), 2);
    EXPECT_EQ(result.getStatement()->getColumns()[0].getColumnName(), "id");
    EXPECT_EQ(result.getStatement()->getColumns()[1].getColumnName(), "name");
}

TEST(ParserTests, ParseWildcardSelectStatement) {
    ParseResult result = parseSql("SELECT * FROM users;");

    ASSERT_TRUE(result.succeeded()) << result.getErrorMessage();
    ASSERT_EQ(result.getStatement()->getColumns().size(), 1);
    EXPECT_TRUE(result.getStatement()->getColumns()[0].isWildcard());
    EXPECT_TRUE(result.getStatement()->isWildcardSelect());
}

TEST(ParserTests, ParseRejectsEmptyTokenStream) {
    Parser parser;

    ParseResult result = parser.parseSelect({});

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected SELECT statement, but token stream was empty");
}

TEST(ParserTests, ParseRejectsStatementWithoutSelectKeyword) {
    Parser parser;
    std::vector<Token> tokens = {
        Token(TokenType::IDENTIFIER, "id"),
        Token(TokenType::FROM, "FROM"),
        Token(TokenType::IDENTIFIER, "users"),
        Token(TokenType::SEMICOLON, ";")
    };

    ParseResult result = parser.parseSelect(tokens);

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected SELECT at start of statement");
}

TEST(ParserTests, ParseRejectsMissingProjectionList) {
    ParseResult result = parseSql("SELECT FROM users;");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected column name in SELECT projection list");
}

TEST(ParserTests, ParseRejectsMissingFromKeyword) {
    ParseResult result = parseSql("SELECT id users;");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected FROM after SELECT projection list");
}

TEST(ParserTests, ParseRejectsMissingTableName) {
    ParseResult result = parseSql("SELECT id FROM ;");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected table name after FROM");
}

TEST(ParserTests, ParseRejectsMissingSemicolon) {
    ParseResult result = parseSql("SELECT id FROM users");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected semicolon at end of SELECT statement");
}

TEST(ParserTests, ParseRejectsTrailingCommaBeforeFrom) {
    ParseResult result = parseSql("SELECT id, FROM users;");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected column name in SELECT projection list");
}

TEST(ParserTests, ParseRejectsWildcardMixedWithNamedColumns) {
    ParseResult result = parseSql("SELECT *, id FROM users;");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Wildcard projection must be the only selected column");
}

TEST(ParserTests, ParseRejectsExtraTokensAfterSemicolon) {
    Parser parser;
    std::vector<Token> tokens = {
        Token(TokenType::SELECT, "SELECT"),
        Token(TokenType::IDENTIFIER, "id"),
        Token(TokenType::FROM, "FROM"),
        Token(TokenType::IDENTIFIER, "users"),
        Token(TokenType::SEMICOLON, ";"),
        Token(TokenType::IDENTIFIER, "extra")
    };

    ParseResult result = parser.parseSelect(tokens);

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Unexpected tokens after end of SELECT statement");
}

TEST(ParserTests, ParseRejectsTokenizerFailureBeforeParsing) {
    ParseResult result = parseSql("SELECT @ FROM users;");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Tokenizer failed");
}

TEST(ParserTests, ParseInsertStatementWithIntegerAndStringValues) {
    InsertParseResult result = parseInsertSql("INSERT INTO users VALUES (1, 'mark');");

    ASSERT_TRUE(result.succeeded()) << result.getErrorMessage();
    ASSERT_TRUE(result.getStatement().has_value());
    EXPECT_EQ(result.getStatement()->getTableName(), "users");
    EXPECT_EQ(result.getStatement()->getValues(), (std::vector<std::string>{"1", "mark"}));
}

TEST(ParserTests, ParseInsertRejectsMissingIntoKeyword) {
    InsertParseResult result = parseInsertSql("INSERT users VALUES (1, 'mark');");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected INTO after INSERT");
}

TEST(ParserTests, ParseInsertRejectsMissingValuesKeyword) {
    InsertParseResult result = parseInsertSql("INSERT INTO users (1, 'mark');");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected VALUES after INSERT target table");
}

TEST(ParserTests, ParseInsertRejectsMissingOpeningParenthesis) {
    InsertParseResult result = parseInsertSql("INSERT INTO users VALUES 1, 'mark');");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected opening parenthesis after VALUES");
}

TEST(ParserTests, ParseInsertRejectsMissingLiteralValue) {
    InsertParseResult result = parseInsertSql("INSERT INTO users VALUES ();");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected literal value inside VALUES list");
}

TEST(ParserTests, ParseInsertRejectsTrailingComma) {
    InsertParseResult result = parseInsertSql("INSERT INTO users VALUES (1,);");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected literal value inside VALUES list");
}

TEST(ParserTests, ParseInsertRejectsMissingClosingParenthesis) {
    InsertParseResult result = parseInsertSql("INSERT INTO users VALUES (1, 'mark';");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected closing parenthesis after VALUES list");
}

TEST(ParserTests, ParseInsertRejectsMissingSemicolon) {
    InsertParseResult result = parseInsertSql("INSERT INTO users VALUES (1, 'mark')");

    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(result.getErrorMessage(), "Expected semicolon at end of INSERT statement");
}
