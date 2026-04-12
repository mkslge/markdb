//
// Created by Mark on 4/12/26.
//

#include "ResultRow.h"
#include <utility>

ResultRow::ResultRow(std::vector<std::string> values) : values_(std::move(values)) {
}

const std::vector<std::string>& ResultRow::getValues() const {
    return values_;
}
