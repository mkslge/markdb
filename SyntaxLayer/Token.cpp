//
// Created by Mark on 4/12/26.
//

#include "Token.h"
#include <utility>

Token::Token(TokenType type, std::string lexeme) : type_(type), lexeme_(std::move(lexeme)) {
}

TokenType Token::getType() const {
    return type_;
}

const std::string& Token::getLexeme() const {
    return lexeme_;
}
