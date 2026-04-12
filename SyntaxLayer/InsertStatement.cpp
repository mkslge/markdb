//
// Created by Mark on 4/12/26.
//

#include "InsertStatement.h"
#include <utility>

InsertStatement::InsertStatement(std::string table_name, std::vector<std::string> values)
    : table_name_(std::move(table_name)), values_(std::move(values)) {
}

const std::string& InsertStatement::getTableName() const {
    return table_name_;
}

const std::vector<std::string>& InsertStatement::getValues() const {
    return values_;
}
