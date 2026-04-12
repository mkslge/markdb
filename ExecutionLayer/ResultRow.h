//
// Created by Mark on 4/12/26.
//

#ifndef RESULTROW_H
#define RESULTROW_H

#include <string>
#include <vector>

class ResultRow {
private:
    std::vector<std::string> values_;

public:
    explicit ResultRow(std::vector<std::string> values);

    const std::vector<std::string>& getValues() const;
};

#endif //RESULTROW_H
