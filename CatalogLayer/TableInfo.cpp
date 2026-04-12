//
// Created by Mark on 4/12/26.
//

#include "TableInfo.h"
#include <utility>

TableInfo::TableInfo(std::string table_name, Schema schema, std::unique_ptr<TableHeap> table_heap)
    : table_name_(std::move(table_name)),
      schema_(std::move(schema)),
      table_heap_(std::move(table_heap)) {
}

const std::string& TableInfo::getTableName() const {
    return table_name_;
}

const Schema& TableInfo::getSchema() const {
    return schema_;
}

TableHeap* TableInfo::getTableHeap() const {
    return table_heap_.get();
}
