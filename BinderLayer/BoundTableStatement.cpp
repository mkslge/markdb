//
// Created by Mark on 4/12/26.
//

#include "BoundTableStatement.h"
#include <utility>

BoundTableStatement::BoundTableStatement(TableInfo* table_info, std::vector<BoundColumnRef> columns)
    : table_info_(table_info), columns_(std::move(columns)) {
}

TableInfo* BoundTableStatement::getTableInfo() const {
    return table_info_;
}

const std::vector<BoundColumnRef>& BoundTableStatement::getColumns() const {
    return columns_;
}
