//
// Created by Mark on 4/12/26.
//

#ifndef PARSER_H
#define PARSER_H

#include "InsertParseResult.h"
#include "ParseResult.h"
#include "Token.h"
#include <vector>

class Parser {
public:
    ParseResult parseSelect(const std::vector<Token>& tokens) const;
    InsertParseResult parseInsert(const std::vector<Token>& tokens) const;
};

#endif //PARSER_H
