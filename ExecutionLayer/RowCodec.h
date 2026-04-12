//
// Created by Mark on 4/12/26.
//

#ifndef ROWCODEC_H
#define ROWCODEC_H

#include "MaterializedRow.h"
#include "../CatalogLayer/Schema.h"
#include "../Models/Tuple.h"
#include <optional>
#include <string>
#include <vector>

class RowCodec {
public:
    static std::optional<Tuple> serialize(const std::vector<std::string>& values);
    static std::optional<std::vector<std::string>> deserialize(const Tuple& tuple, std::size_t expected_columns);
    static std::optional<MaterializedRow> materialize(const Tuple& tuple, const Schema& schema);

private:
    static constexpr char FIELD_SEPARATOR = '\x1F';
};

#endif //ROWCODEC_H
