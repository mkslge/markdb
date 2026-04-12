//
// Created by Mark on 4/12/26.
//

#include "BoundColumnRef.h"

BoundColumnRef::BoundColumnRef(const Column* column, std::size_t column_index)
    : column_(column), column_index_(column_index) {
}

const Column* BoundColumnRef::getColumn() const {
    return column_;
}

std::size_t BoundColumnRef::getColumnIndex() const {
    return column_index_;
}
