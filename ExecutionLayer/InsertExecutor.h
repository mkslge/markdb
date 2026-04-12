//
// Created by Mark on 4/12/26.
//

#ifndef INSERTEXECUTOR_H
#define INSERTEXECUTOR_H

#include "../BinderLayer/BoundInsertStatement.h"
#include <optional>

class InsertExecutor {
public:
    std::optional<RID> execute(const BoundInsertStatement& statement) const;
};

#endif //INSERTEXECUTOR_H
