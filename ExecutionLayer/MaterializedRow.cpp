//
// Created by Mark on 4/12/26.
//

#include "MaterializedRow.h"
#include <utility>

MaterializedRow::MaterializedRow(std::vector<std::string> values) : values_(std::move(values)) {
}

std::size_t MaterializedRow::size() const {
    return values_.size();
}

const std::vector<std::string>& MaterializedRow::getValues() const {
    return values_;
}

std::optional<std::string> MaterializedRow::getValue(std::size_t index) const {
    if (index >= values_.size()) {
        return std::nullopt;
    }

    return values_[index];
}
