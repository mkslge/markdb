//
// Created by Mark on 4/12/26.
//

#ifndef CATALOG_H
#define CATALOG_H

#include "TableInfo.h"
#include "../DiskLayer/DiskManager.h"
#include "../MemoryManagementLayer/BufferPoolManager.h"
#include <string>
#include <unordered_map>
#include <vector>

class Catalog {
private:
    DiskManager* dm_;
    BufferPoolManager* bpm_;
    std::unordered_map<std::string, std::unique_ptr<TableInfo>> tables_;

public:
    Catalog(DiskManager* dm, BufferPoolManager* bpm);

    TableInfo* createTable(const std::string& table_name, const Schema& schema);
    TableInfo* loadTable(const std::string& table_name, const Schema& schema, int first_page_id);
    TableInfo* getTable(const std::string& table_name) const;
    bool hasTable(const std::string& table_name) const;
    std::vector<std::string> listTables() const;

private:
    static bool hasDuplicateColumns(const Schema& schema);
};

#endif //CATALOG_H
