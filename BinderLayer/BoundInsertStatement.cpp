//
// Created by Mark on 4/12/26.
//

#include "BoundInsertStatement.h"
#include <utility>

BoundInsertStatement::BoundInsertStatement(TableInfo* table_info, std::vector<std::string> values)
    : table_info_(table_info), values_(std::move(values)) {
}

TableInfo* BoundInsertStatement::getTableInfo() const {
    return table_info_;
}

const std::vector<std::string>& BoundInsertStatement::getValues() const {
    return values_;
}
