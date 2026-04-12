//
// Created by Mark on 4/12/26.
//

#include "Catalog.h"
#include <memory>

Catalog::Catalog(DiskManager* dm, BufferPoolManager* bpm) : dm_(dm), bpm_(bpm) {
}

TableInfo* Catalog::createTable(const std::string& table_name, const Schema& schema) {
    if (table_name.empty() || schema.getColumnCount() == 0 || hasDuplicateColumns(schema) ||
        tables_.contains(table_name)) {
        return nullptr;
    }

    auto table_heap = std::make_unique<TableHeap>(dm_, bpm_);
    auto table_info = std::make_unique<TableInfo>(table_name, schema, std::move(table_heap));
    TableInfo* table_info_ptr = table_info.get();
    tables_.emplace(table_name, std::move(table_info));
    return table_info_ptr;
}

TableInfo* Catalog::loadTable(const std::string& table_name, const Schema& schema, int first_page_id) {
    if (table_name.empty() || schema.getColumnCount() == 0 || hasDuplicateColumns(schema) ||
        tables_.contains(table_name) || first_page_id < 0) {
        return nullptr;
    }

    auto table_heap = std::make_unique<TableHeap>(dm_, bpm_, first_page_id);
    auto table_info = std::make_unique<TableInfo>(table_name, schema, std::move(table_heap));
    TableInfo* table_info_ptr = table_info.get();
    tables_.emplace(table_name, std::move(table_info));
    return table_info_ptr;
}

TableInfo* Catalog::getTable(const std::string& table_name) const {
    auto iterator = tables_.find(table_name);
    if (iterator == tables_.end()) {
        return nullptr;
    }
    return iterator->second.get();
}

bool Catalog::hasTable(const std::string& table_name) const {
    return tables_.contains(table_name);
}

std::vector<std::string> Catalog::listTables() const {
    std::vector<std::string> table_names;
    table_names.reserve(tables_.size());

    for (const auto& [table_name, table_info] : tables_) {
        (void)table_info;
        table_names.push_back(table_name);
    }

    return table_names;
}

bool Catalog::hasDuplicateColumns(const Schema& schema) {
    std::unordered_map<std::string, bool> seen_names;

    for (const Column& column : schema.getColumns()) {
        if (column.getName().empty() || seen_names.contains(column.getName())) {
            return true;
        }
        seen_names.emplace(column.getName(), true);
    }

    return false;
}
