//
// Created by Mark on 4/12/26.
//

#ifndef MATERIALIZEDROW_H
#define MATERIALIZEDROW_H

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class MaterializedRow {
private:
    std::vector<std::string> values_;

public:
    explicit MaterializedRow(std::vector<std::string> values);

    std::size_t size() const;
    const std::vector<std::string>& getValues() const;
    std::optional<std::string> getValue(std::size_t index) const;
};

#endif //MATERIALIZEDROW_H
