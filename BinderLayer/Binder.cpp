//
// Created by Mark on 4/12/26.
//

#include "Binder.h"

Binder::Binder(Catalog* catalog) : catalog_(catalog) {
}

std::optional<BoundInsertStatement> Binder::bindInsert(const InsertStatement& statement) const {
    if (catalog_ == nullptr) {
        return std::nullopt;
    }

    TableInfo* table_info = catalog_->getTable(statement.getTableName());
    if (table_info == nullptr) {
        return std::nullopt;
    }

    if (table_info->getSchema().getColumnCount() != statement.getValues().size()) {
        return std::nullopt;
    }

    return BoundInsertStatement(table_info, statement.getValues());
}

std::optional<BoundTableStatement> Binder::bindSelect(const SelectStatement& statement) const {
    std::vector<std::string> column_names;
    column_names.reserve(statement.getColumns().size());

    for (const ColumnExpression& column : statement.getColumns()) {
        if (column.isWildcard()) {
            column_names.emplace_back("*");
            continue;
        }

        column_names.push_back(column.getColumnName());
    }

    return bindTableColumns(statement.getTableName(), column_names);
}

std::optional<BoundTableStatement> Binder::bindTableColumns(
        const std::string& table_name, const std::vector<std::string>& column_names) const {
    if (catalog_ == nullptr) {
        return std::nullopt;
    }

    TableInfo* table_info = catalog_->getTable(table_name);
    if (table_info == nullptr) {
        return std::nullopt;
    }

    const Schema& schema = table_info->getSchema();
    std::vector<BoundColumnRef> bound_columns;

    if (requestsWildcard(column_names)) {
        bound_columns.reserve(schema.getColumnCount());
        for (std::size_t i = 0; i < schema.getColumnCount(); i++) {
            bound_columns.emplace_back(&schema.getColumn(i), i);
        }
        return BoundTableStatement(table_info, std::move(bound_columns));
    }

    bound_columns.reserve(column_names.size());
    for (const std::string& column_name : column_names) {
        std::optional<std::size_t> column_index = schema.getColumnIndex(column_name);
        if (!column_index.has_value()) {
            return std::nullopt;
        }

        bound_columns.emplace_back(&schema.getColumn(column_index.value()), column_index.value());
    }

    return BoundTableStatement(table_info, std::move(bound_columns));
}

bool Binder::requestsWildcard(const std::vector<std::string>& column_names) {
    if (column_names.empty()) {
        return true;
    }

    if (column_names.size() == 1 && column_names.front() == "*") {
        return true;
    }

    for (const std::string& column_name : column_names) {
        if (column_name == "*") {
            return false;
        }
    }

    return false;
}
