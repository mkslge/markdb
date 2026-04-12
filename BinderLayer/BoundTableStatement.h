//
// Created by Mark on 4/12/26.
//

#ifndef BOUNDTABLESTATEMENT_H
#define BOUNDTABLESTATEMENT_H

#include "BoundColumnRef.h"
#include "../CatalogLayer/TableInfo.h"
#include <vector>

class BoundTableStatement {
private:
    TableInfo* table_info_;
    std::vector<BoundColumnRef> columns_;

public:
    BoundTableStatement(TableInfo* table_info, std::vector<BoundColumnRef> columns);

    TableInfo* getTableInfo() const;
    const std::vector<BoundColumnRef>& getColumns() const;
};

#endif //BOUNDTABLESTATEMENT_H
