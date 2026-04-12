//
// Created by Mark on 4/12/26.
//

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "Token.h"
#include <optional>
#include <string>
#include <vector>

class Tokenizer {
public:
    std::optional<std::vector<Token>> tokenize(const std::string& input) const;

private:
    static bool isIdentifierStart(char c);
    static bool isIdentifierPart(char c);
    static TokenType classifyIdentifier(const std::string& lexeme);
    static std::optional<std::string> readStringLiteral(const std::string& input, std::size_t& index);
    static std::string readIntegerLiteral(const std::string& input, std::size_t& index);
};

#endif //TOKENIZER_H
