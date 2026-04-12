//
// Created by Mark on 4/12/26.
//

#include "ColumnExpression.h"
#include <utility>

ColumnExpression::ColumnExpression(std::string column_name)
    : column_name_(std::move(column_name)), is_wildcard_(false) {
}

ColumnExpression ColumnExpression::wildcard() {
    ColumnExpression expression("*");
    expression.is_wildcard_ = true;
    return expression;
}

const std::string& ColumnExpression::getColumnName() const {
    return column_name_;
}

bool ColumnExpression::isWildcard() const {
    return is_wildcard_;
}
