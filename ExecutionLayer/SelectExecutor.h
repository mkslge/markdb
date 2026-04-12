//
// Created by Mark on 4/12/26.
//

#ifndef SELECTEXECUTOR_H
#define SELECTEXECUTOR_H

#include "../BinderLayer/BoundTableStatement.h"
#include "ResultRow.h"
#include <optional>
#include <vector>

class SelectExecutor {
public:
    std::optional<std::vector<ResultRow>> execute(const BoundTableStatement& statement) const;
};

#endif //SELECTEXECUTOR_H
