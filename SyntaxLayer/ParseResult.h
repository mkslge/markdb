//
// Created by Mark on 4/12/26.
//

#ifndef PARSERESULT_H
#define PARSERESULT_H

#include "SelectStatement.h"
#include <optional>
#include <string>

class ParseResult {
private:
    std::optional<SelectStatement> statement_;
    std::string error_message_;

public:
    ParseResult(std::optional<SelectStatement> statement, std::string error_message);

    static ParseResult success(SelectStatement statement);
    static ParseResult failure(std::string error_message);

    bool succeeded() const;
    const std::optional<SelectStatement>& getStatement() const;
    const std::string& getErrorMessage() const;
};

#endif //PARSERESULT_H
