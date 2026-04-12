//
// Created by Mark on 4/12/26.
//

#include "Parser.h"

ParseResult Parser::parseSelect(const std::vector<Token>& tokens) const {
    if (tokens.empty()) {
        return ParseResult::failure("Expected SELECT statement, but token stream was empty");
    }

    std::size_t current = 0;

    if (tokens[current].getType() != TokenType::SELECT) {
        return ParseResult::failure("Expected SELECT at start of statement");
    }
    current++;

    std::vector<ColumnExpression> columns;

    if (current >= tokens.size()) {
        return ParseResult::failure("Expected projection list after SELECT");
    }

    if (tokens[current].getType() == TokenType::STAR) {
        columns.push_back(ColumnExpression::wildcard());
        current++;

        if (current < tokens.size() && tokens[current].getType() == TokenType::COMMA) {
            return ParseResult::failure("Wildcard projection must be the only selected column");
        }
    } else {
        while (true) {
            if (current >= tokens.size() || tokens[current].getType() != TokenType::IDENTIFIER) {
                return ParseResult::failure("Expected column name in SELECT projection list");
            }

            columns.emplace_back(tokens[current].getLexeme());
            current++;

            if (current < tokens.size() && tokens[current].getType() == TokenType::COMMA) {
                current++;
                continue;
            }

            break;
        }
    }

    if (current >= tokens.size() || tokens[current].getType() != TokenType::FROM) {
        return ParseResult::failure("Expected FROM after SELECT projection list");
    }
    current++;

    if (current >= tokens.size() || tokens[current].getType() != TokenType::IDENTIFIER) {
        return ParseResult::failure("Expected table name after FROM");
    }

    std::string table_name = tokens[current].getLexeme();
    current++;

    if (current >= tokens.size() || tokens[current].getType() != TokenType::SEMICOLON) {
        return ParseResult::failure("Expected semicolon at end of SELECT statement");
    }
    current++;

    if (current != tokens.size()) {
        return ParseResult::failure("Unexpected tokens after end of SELECT statement");
    }

    return ParseResult::success(SelectStatement(std::move(columns), std::move(table_name)));
}

InsertParseResult Parser::parseInsert(const std::vector<Token>& tokens) const {
    if (tokens.empty()) {
        return InsertParseResult::failure("Expected INSERT statement, but token stream was empty");
    }

    std::size_t current = 0;

    if (tokens[current].getType() != TokenType::INSERT) {
        return InsertParseResult::failure("Expected INSERT at start of statement");
    }
    current++;

    if (current >= tokens.size() || tokens[current].getType() != TokenType::INTO) {
        return InsertParseResult::failure("Expected INTO after INSERT");
    }
    current++;

    if (current >= tokens.size() || tokens[current].getType() != TokenType::IDENTIFIER) {
        return InsertParseResult::failure("Expected table name after INTO");
    }
    std::string table_name = tokens[current].getLexeme();
    current++;

    if (current >= tokens.size() || tokens[current].getType() != TokenType::VALUES) {
        return InsertParseResult::failure("Expected VALUES after INSERT target table");
    }
    current++;

    if (current >= tokens.size() || tokens[current].getType() != TokenType::LEFT_PAREN) {
        return InsertParseResult::failure("Expected opening parenthesis after VALUES");
    }
    current++;

    std::vector<std::string> values;

    while (true) {
        if (current >= tokens.size() ||
            (tokens[current].getType() != TokenType::STRING_LITERAL &&
             tokens[current].getType() != TokenType::INTEGER_LITERAL)) {
            return InsertParseResult::failure("Expected literal value inside VALUES list");
        }

        values.push_back(tokens[current].getLexeme());
        current++;

        if (current < tokens.size() && tokens[current].getType() == TokenType::COMMA) {
            current++;
            continue;
        }

        break;
    }

    if (current >= tokens.size() || tokens[current].getType() != TokenType::RIGHT_PAREN) {
        return InsertParseResult::failure("Expected closing parenthesis after VALUES list");
    }
    current++;

    if (current >= tokens.size() || tokens[current].getType() != TokenType::SEMICOLON) {
        return InsertParseResult::failure("Expected semicolon at end of INSERT statement");
    }
    current++;

    if (current != tokens.size()) {
        return InsertParseResult::failure("Unexpected tokens after end of INSERT statement");
    }

    return InsertParseResult::success(InsertStatement(std::move(table_name), std::move(values)));
}
