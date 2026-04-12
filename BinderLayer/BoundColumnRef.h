//
// Created by Mark on 4/12/26.
//

#ifndef BOUNDCOLUMNREF_H
#define BOUNDCOLUMNREF_H

#include "../CatalogLayer/Column.h"
#include <cstddef>

class BoundColumnRef {
private:
    const Column* column_;
    std::size_t column_index_;

public:
    BoundColumnRef(const Column* column, std::size_t column_index);

    const Column* getColumn() const;
    std::size_t getColumnIndex() const;
};

#endif //BOUNDCOLUMNREF_H
