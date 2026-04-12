//
// Created by Mark on 4/12/26.
//

#ifndef BOUNDINSERTSTATEMENT_H
#define BOUNDINSERTSTATEMENT_H

#include "../CatalogLayer/TableInfo.h"
#include <string>
#include <vector>

class BoundInsertStatement {
private:
    TableInfo* table_info_;
    std::vector<std::string> values_;

public:
    BoundInsertStatement(TableInfo* table_info, std::vector<std::string> values);

    TableInfo* getTableInfo() const;
    const std::vector<std::string>& getValues() const;
};

#endif //BOUNDINSERTSTATEMENT_H
