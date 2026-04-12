//
// Created by Mark on 4/12/26.
//

#include "Tokenizer.h"
#include <cctype>

std::optional<std::vector<Token>> Tokenizer::tokenize(const std::string& input) const {
    std::vector<Token> tokens;

    for (std::size_t i = 0; i < input.size();) {
        const unsigned char current = static_cast<unsigned char>(input[i]);

        if (std::isspace(current)) {
            i++;
            continue;
        }

        if (input[i] == '*') {
            tokens.emplace_back(TokenType::STAR, "*");
            i++;
            continue;
        }

        if (input[i] == ',') {
            tokens.emplace_back(TokenType::COMMA, ",");
            i++;
            continue;
        }

        if (input[i] == ';') {
            tokens.emplace_back(TokenType::SEMICOLON, ";");
            i++;
            continue;
        }

        if (input[i] == '(') {
            tokens.emplace_back(TokenType::LEFT_PAREN, "(");
            i++;
            continue;
        }

        if (input[i] == ')') {
            tokens.emplace_back(TokenType::RIGHT_PAREN, ")");
            i++;
            continue;
        }

        if (input[i] == '\'') {
            std::optional<std::string> literal = readStringLiteral(input, i);
            if (!literal.has_value()) {
                return std::nullopt;
            }
            tokens.emplace_back(TokenType::STRING_LITERAL, literal.value());
            continue;
        }

        if (std::isdigit(current)) {
            tokens.emplace_back(TokenType::INTEGER_LITERAL, readIntegerLiteral(input, i));
            continue;
        }

        if (isIdentifierStart(input[i])) {
            std::size_t start = i;
            i++;

            while (i < input.size() && isIdentifierPart(input[i])) {
                i++;
            }

            std::string lexeme = input.substr(start, i - start);
            tokens.emplace_back(classifyIdentifier(lexeme), lexeme);
            continue;
        }

        return std::nullopt;
    }

    return tokens;
}

bool Tokenizer::isIdentifierStart(char c) {
    const unsigned char value = static_cast<unsigned char>(c);
    return std::isalpha(value) || c == '_';
}

bool Tokenizer::isIdentifierPart(char c) {
    const unsigned char value = static_cast<unsigned char>(c);
    return std::isalnum(value) || c == '_';
}

TokenType Tokenizer::classifyIdentifier(const std::string& lexeme) {
    std::string uppercased;
    uppercased.reserve(lexeme.size());

    for (char c : lexeme) {
        uppercased.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }

    if (uppercased == "SELECT") {
        return TokenType::SELECT;
    }

    if (uppercased == "FROM") {
        return TokenType::FROM;
    }

    if (uppercased == "INSERT") {
        return TokenType::INSERT;
    }

    if (uppercased == "INTO") {
        return TokenType::INTO;
    }

    if (uppercased == "VALUES") {
        return TokenType::VALUES;
    }

    return TokenType::IDENTIFIER;
}

std::optional<std::string> Tokenizer::readStringLiteral(const std::string& input, std::size_t& index) {
    index++;
    std::string literal;

    while (index < input.size() && input[index] != '\'') {
        literal.push_back(input[index]);
        index++;
    }

    if (index >= input.size()) {
        return std::nullopt;
    }

    index++;
    return literal;
}

std::string Tokenizer::readIntegerLiteral(const std::string& input, std::size_t& index) {
    std::size_t start = index;
    while (index < input.size() && std::isdigit(static_cast<unsigned char>(input[index]))) {
        index++;
    }
    return input.substr(start, index - start);
}
