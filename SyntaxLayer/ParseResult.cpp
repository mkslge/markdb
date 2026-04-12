//
// Created by Mark on 4/12/26.
//

#include "ParseResult.h"
#include <utility>

ParseResult::ParseResult(std::optional<SelectStatement> statement, std::string error_message)
    : statement_(std::move(statement)), error_message_(std::move(error_message)) {
}

ParseResult ParseResult::success(SelectStatement statement) {
    return ParseResult(std::move(statement), "");
}

ParseResult ParseResult::failure(std::string error_message) {
    return ParseResult(std::nullopt, std::move(error_message));
}

bool ParseResult::succeeded() const {
    return statement_.has_value();
}

const std::optional<SelectStatement>& ParseResult::getStatement() const {
    return statement_;
}

const std::string& ParseResult::getErrorMessage() const {
    return error_message_;
}
