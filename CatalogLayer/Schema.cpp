//
// Created by Mark on 4/12/26.
//

#include "Schema.h"
#include <stdexcept>
#include <utility>

Schema::Schema(std::vector<Column> columns) : columns_(std::move(columns)) {
}

std::size_t Schema::getColumnCount() const {
    return columns_.size();
}

const Column& Schema::getColumn(std::size_t index) const {
    if (index >= columns_.size()) {
        throw std::out_of_range("Schema column index out of range");
    }
    return columns_[index];
}

const std::vector<Column>& Schema::getColumns() const {
    return columns_;
}

std::optional<std::size_t> Schema::getColumnIndex(const std::string& column_name) const {
    for (std::size_t i = 0; i < columns_.size(); i++) {
        if (columns_[i].getName() == column_name) {
            return i;
        }
    }
    return std::nullopt;
}

bool Schema::hasColumn(const std::string& column_name) const {
    return getColumnIndex(column_name).has_value();
}
