//
// Created by Mark on 4/12/26.
//

#ifndef SCHEMA_H
#define SCHEMA_H

#include "Column.h"
#include <cstddef>
#include <optional>
#include <vector>

class Schema {
private:
    std::vector<Column> columns_;

public:
    Schema() = default;
    explicit Schema(std::vector<Column> columns);

    std::size_t getColumnCount() const;
    const Column& getColumn(std::size_t index) const;
    const std::vector<Column>& getColumns() const;
    std::optional<std::size_t> getColumnIndex(const std::string& column_name) const;
    bool hasColumn(const std::string& column_name) const;
};

#endif //SCHEMA_H
