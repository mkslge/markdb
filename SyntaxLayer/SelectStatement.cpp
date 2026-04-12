//
// Created by Mark on 4/12/26.
//

#include "SelectStatement.h"
#include <utility>

SelectStatement::SelectStatement(std::vector<ColumnExpression> columns, std::string table_name)
    : columns_(std::move(columns)), table_name_(std::move(table_name)) {
}

const std::vector<ColumnExpression>& SelectStatement::getColumns() const {
    return columns_;
}

const std::string& SelectStatement::getTableName() const {
    return table_name_;
}

bool SelectStatement::isWildcardSelect() const {
    return columns_.size() == 1 && columns_.front().isWildcard();
}
