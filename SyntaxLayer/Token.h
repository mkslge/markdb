//
// Created by Mark on 4/12/26.
//

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    SELECT,
    FROM,
    INSERT,
    INTO,
    VALUES,
    IDENTIFIER,
    STRING_LITERAL,
    INTEGER_LITERAL,
    STAR,
    COMMA,
    SEMICOLON,
    LEFT_PAREN,
    RIGHT_PAREN
};

class Token {
private:
    TokenType type_;
    std::string lexeme_;

public:
    Token(TokenType type, std::string lexeme);

    TokenType getType() const;
    const std::string& getLexeme() const;
};

#endif //TOKEN_H
