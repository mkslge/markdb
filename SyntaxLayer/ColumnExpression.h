//
// Created by Mark on 4/12/26.
//

#ifndef COLUMNEXPRESSION_H
#define COLUMNEXPRESSION_H

#include <string>

class ColumnExpression {
private:
    std::string column_name_;
    bool is_wildcard_;

public:
    explicit ColumnExpression(std::string column_name);

    static ColumnExpression wildcard();

    const std::string& getColumnName() const;
    bool isWildcard() const;
};

#endif //COLUMNEXPRESSION_H
