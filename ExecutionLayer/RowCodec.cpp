//
// Created by Mark on 4/12/26.
//

#include "RowCodec.h"

std::optional<Tuple> RowCodec::serialize(const std::vector<std::string>& values) {
    Tuple tuple;

    for (std::size_t i = 0; i < values.size(); i++) {
        for (char c : values[i]) {
            if (c == FIELD_SEPARATOR) {
                return std::nullopt;
            }
            tuple.data_.push_back(c);
        }

        if (i + 1 < values.size()) {
            tuple.data_.push_back(FIELD_SEPARATOR);
        }
    }

    return tuple;
}

std::optional<std::vector<std::string>> RowCodec::deserialize(const Tuple& tuple, std::size_t expected_columns) {
    std::vector<std::string> values;
    values.emplace_back();

    for (char c : tuple.data_) {
        if (c == FIELD_SEPARATOR) {
            values.emplace_back();
            continue;
        }
        values.back().push_back(c);
    }

    if (expected_columns == 0) {
        if (tuple.data_.empty()) {
            return std::vector<std::string>{};
        }
        return std::nullopt;
    }

    if (values.size() != expected_columns) {
        return std::nullopt;
    }

    return values;
}

std::optional<MaterializedRow> RowCodec::materialize(const Tuple& tuple, const Schema& schema) {
    std::optional<std::vector<std::string>> values = deserialize(tuple, schema.getColumnCount());
    if (!values.has_value()) {
        return std::nullopt;
    }

    return MaterializedRow(std::move(values.value()));
}
