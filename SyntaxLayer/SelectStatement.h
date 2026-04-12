//
// Created by Mark on 4/12/26.
//

#ifndef SELECTSTATEMENT_H
#define SELECTSTATEMENT_H

#include "ColumnExpression.h"
#include <string>
#include <vector>

class SelectStatement {
private:
    std::vector<ColumnExpression> columns_;
    std::string table_name_;

public:
    SelectStatement(std::vector<ColumnExpression> columns, std::string table_name);

    const std::vector<ColumnExpression>& getColumns() const;
    const std::string& getTableName() const;
    bool isWildcardSelect() const;
};

#endif //SELECTSTATEMENT_H
