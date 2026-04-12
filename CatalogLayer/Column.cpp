//
// Created by Mark on 4/12/26.
//

#include "Column.h"
#include <utility>

Column::Column(std::string name, TypeId type, bool nullable)
    : name_(std::move(name)), type_(type), nullable_(nullable) {
}

const std::string& Column::getName() const {
    return name_;
}

TypeId Column::getType() const {
    return type_;
}

bool Column::isNullable() const {
    return nullable_;
}
