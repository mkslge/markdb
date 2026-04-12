//
// Created by Mark on 4/12/26.
//

#include "InsertParseResult.h"
#include <utility>

InsertParseResult::InsertParseResult(std::optional<InsertStatement> statement, std::string error_message)
    : statement_(std::move(statement)), error_message_(std::move(error_message)) {
}

InsertParseResult InsertParseResult::success(InsertStatement statement) {
    return InsertParseResult(std::move(statement), "");
}

InsertParseResult InsertParseResult::failure(std::string error_message) {
    return InsertParseResult(std::nullopt, std::move(error_message));
}

bool InsertParseResult::succeeded() const {
    return statement_.has_value();
}

const std::optional<InsertStatement>& InsertParseResult::getStatement() const {
    return statement_;
}

const std::string& InsertParseResult::getErrorMessage() const {
    return error_message_;
}
