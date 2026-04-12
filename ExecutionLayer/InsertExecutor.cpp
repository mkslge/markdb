//
// Created by Mark on 4/12/26.
//

#include "InsertExecutor.h"
#include "RowCodec.h"

std::optional<RID> InsertExecutor::execute(const BoundInsertStatement& statement) const {
    if (statement.getTableInfo() == nullptr || statement.getTableInfo()->getTableHeap() == nullptr) {
        return std::nullopt;
    }

    std::optional<Tuple> tuple = RowCodec::serialize(statement.getValues());
    if (!tuple.has_value()) {
        return std::nullopt;
    }

    RID rid = statement.getTableInfo()->getTableHeap()->insertTuple(tuple.value());
    if (rid.page_id < 0 || rid.slot_id < 0) {
        return std::nullopt;
    }

    return rid;
}
