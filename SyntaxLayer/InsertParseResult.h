//
// Created by Mark on 4/12/26.
//

#ifndef INSERTPARSERESULT_H
#define INSERTPARSERESULT_H

#include "InsertStatement.h"
#include <optional>
#include <string>

class InsertParseResult {
private:
    std::optional<InsertStatement> statement_;
    std::string error_message_;

public:
    InsertParseResult(std::optional<InsertStatement> statement, std::string error_message);

    static InsertParseResult success(InsertStatement statement);
    static InsertParseResult failure(std::string error_message);

    bool succeeded() const;
    const std::optional<InsertStatement>& getStatement() const;
    const std::string& getErrorMessage() const;
};

#endif //INSERTPARSERESULT_H
