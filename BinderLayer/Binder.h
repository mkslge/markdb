//
// Created by Mark on 4/12/26.
//

#ifndef BINDER_H
#define BINDER_H

#include "BoundInsertStatement.h"
#include "BoundTableStatement.h"
#include "../CatalogLayer/Catalog.h"
#include "../SyntaxLayer/InsertStatement.h"
#include "../SyntaxLayer/SelectStatement.h"
#include <optional>
#include <string>
#include <vector>

class Binder {
private:
    Catalog* catalog_;

public:
    explicit Binder(Catalog* catalog);

    std::optional<BoundInsertStatement> bindInsert(const InsertStatement& statement) const;
    std::optional<BoundTableStatement> bindSelect(const SelectStatement& statement) const;
    std::optional<BoundTableStatement> bindTableColumns(const std::string& table_name,
                                                        const std::vector<std::string>& column_names) const;

private:
    static bool requestsWildcard(const std::vector<std::string>& column_names);
};

#endif //BINDER_H
