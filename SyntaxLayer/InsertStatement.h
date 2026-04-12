//
// Created by Mark on 4/12/26.
//

#ifndef INSERTSTATEMENT_H
#define INSERTSTATEMENT_H

#include <string>
#include <vector>

class InsertStatement {
private:
    std::string table_name_;
    std::vector<std::string> values_;

public:
    InsertStatement(std::string table_name, std::vector<std::string> values);

    const std::string& getTableName() const;
    const std::vector<std::string>& getValues() const;
};

#endif //INSERTSTATEMENT_H
