//
// Created by Mark on 4/12/26.
//

#ifndef COLUMN_H
#define COLUMN_H

#include <string>

enum class TypeId {
    INTEGER,
    TEXT
};

class Column {
private:
    std::string name_;
    TypeId type_;
    bool nullable_;

public:
    Column(std::string name, TypeId type, bool nullable = true);

    const std::string& getName() const;
    TypeId getType() const;
    bool isNullable() const;
};

#endif //COLUMN_H
