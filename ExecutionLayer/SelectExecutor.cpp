//
// Created by Mark on 4/12/26.
//

#include "SelectExecutor.h"
#include "RowCodec.h"

std::optional<std::vector<ResultRow>> SelectExecutor::execute(const BoundTableStatement& statement) const {
    if (statement.getTableInfo() == nullptr || statement.getTableInfo()->getTableHeap() == nullptr) {
        return std::nullopt;
    }

    const Schema& schema = statement.getTableInfo()->getSchema();
    std::vector<Tuple> tuples = statement.getTableInfo()->getTableHeap()->scanTuples();
    std::vector<ResultRow> rows;
    rows.reserve(tuples.size());

    for (const Tuple& tuple : tuples) {
        std::optional<MaterializedRow> materialized_row = RowCodec::materialize(tuple, schema);
        if (!materialized_row.has_value()) {
            return std::nullopt;
        }

        std::vector<std::string> projected_values;
        projected_values.reserve(statement.getColumns().size());

        for (const BoundColumnRef& column : statement.getColumns()) {
            std::optional<std::string> value = materialized_row->getValue(column.getColumnIndex());
            if (!value.has_value()) {
                return std::nullopt;
            }
            projected_values.push_back(std::move(value.value()));
        }

        rows.emplace_back(std::move(projected_values));
    }

    return rows;
}
