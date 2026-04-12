//
// Created by Mark on 4/12/26.
//

#ifndef TABLEINFO_H
#define TABLEINFO_H

#include "Schema.h"
#include "../TableLayer/TableHeap.h"
#include <memory>
#include <string>

class TableInfo {
private:
    std::string table_name_;
    Schema schema_;
    std::unique_ptr<TableHeap> table_heap_;

public:
    TableInfo(std::string table_name, Schema schema, std::unique_ptr<TableHeap> table_heap);

    const std::string& getTableName() const;
    const Schema& getSchema() const;
    TableHeap* getTableHeap() const;
};

#endif //TABLEINFO_H
