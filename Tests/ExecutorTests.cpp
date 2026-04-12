//
// Created by Mark on 4/12/26.
//

#include <gtest/gtest.h>
#include "../BinderLayer/Binder.h"
#include "../ExecutionLayer/InsertExecutor.h"
#include "../ExecutionLayer/MaterializedRow.h"
#include "../ExecutionLayer/RowCodec.h"
#include "../ExecutionLayer/SelectExecutor.h"

class ExecutorTest : public ::testing::Test {
protected:
    DiskManager dm_{"/Users/markseeliger/CLionProjects/marksql/Storage/TestDBs/executor.db"};
    BufferPoolManager bpm_{&dm_};
    Catalog catalog_{&dm_, &bpm_};
    Binder binder_{&catalog_};
    InsertExecutor insert_executor_;
    SelectExecutor executor_;

    TableInfo* createUsersTable() {
        Schema schema({
            Column("id", TypeId::INTEGER, false),
            Column("name", TypeId::TEXT),
            Column("email", TypeId::TEXT)
        });
        return catalog_.createTable("users", schema);
    }
};

TEST(RowCodecTests, SerializeAndDeserializeRoundTripValues) {
    std::optional<Tuple> tuple = RowCodec::serialize({"1", "mark", "mark@example.com"});

    ASSERT_TRUE(tuple.has_value());
    std::optional<std::vector<std::string>> values = RowCodec::deserialize(tuple.value(), 3);

    ASSERT_TRUE(values.has_value());
    EXPECT_EQ(values.value(), (std::vector<std::string>{"1", "mark", "mark@example.com"}));
}

TEST(RowCodecTests, SerializeRejectsSeparatorInsideValue) {
    EXPECT_FALSE(RowCodec::serialize({"good", std::string("bad\x1Fvalue", 9)}).has_value());
}

TEST(RowCodecTests, DeserializeRejectsWrongColumnCount) {
    std::optional<Tuple> tuple = RowCodec::serialize({"1", "mark", "mark@example.com"});
    ASSERT_TRUE(tuple.has_value());

    EXPECT_FALSE(RowCodec::deserialize(tuple.value(), 2).has_value());
}

TEST(MaterializedRowTests, GetValueReturnsRequestedColumnOrNulloptWhenOutOfRange) {
    MaterializedRow row({"1", "mark", "mark@example.com"});

    EXPECT_EQ(row.getValue(0), std::optional<std::string>("1"));
    EXPECT_EQ(row.getValue(2), std::optional<std::string>("mark@example.com"));
    EXPECT_EQ(row.getValue(3), std::nullopt);
}

TEST(RowCodecTests, MaterializeBuildsSchemaAlignedLogicalRow) {
    Schema schema({
        Column("id", TypeId::INTEGER, false),
        Column("name", TypeId::TEXT),
        Column("email", TypeId::TEXT)
    });

    std::optional<Tuple> tuple = RowCodec::serialize({"1", "mark", "mark@example.com"});
    ASSERT_TRUE(tuple.has_value());

    std::optional<MaterializedRow> row = RowCodec::materialize(tuple.value(), schema);

    ASSERT_TRUE(row.has_value());
    EXPECT_EQ(row->size(), 3);
    EXPECT_EQ(row->getValue(0), std::optional<std::string>("1"));
    EXPECT_EQ(row->getValue(1), std::optional<std::string>("mark"));
    EXPECT_EQ(row->getValues(), (std::vector<std::string>{"1", "mark", "mark@example.com"}));
}

TEST(RowCodecTests, MaterializeRejectsTupleThatDoesNotMatchSchemaShape) {
    Schema schema({
        Column("id", TypeId::INTEGER, false),
        Column("name", TypeId::TEXT)
    });

    std::optional<Tuple> tuple = RowCodec::serialize({"1", "mark", "extra"});
    ASSERT_TRUE(tuple.has_value());

    EXPECT_FALSE(RowCodec::materialize(tuple.value(), schema).has_value());
}

TEST_F(ExecutorTest, ExecuteProjectsRequestedColumnsInBoundOrder) {
    TableInfo* table_info = createUsersTable();
    ASSERT_NE(table_info, nullptr);

    std::optional<Tuple> first = RowCodec::serialize({"1", "mark", "mark@example.com"});
    std::optional<Tuple> second = RowCodec::serialize({"2", "ada", "ada@example.com"});
    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());

    table_info->getTableHeap()->insertTuple(first.value());
    table_info->getTableHeap()->insertTuple(second.value());

    SelectStatement statement({
        ColumnExpression("name"),
        ColumnExpression("id")
    }, "users");

    std::optional<BoundTableStatement> bound = binder_.bindSelect(statement);
    ASSERT_TRUE(bound.has_value());

    std::optional<std::vector<ResultRow>> rows = executor_.execute(bound.value());

    ASSERT_TRUE(rows.has_value());
    ASSERT_EQ(rows->size(), 2);
    EXPECT_EQ(rows->at(0).getValues(), (std::vector<std::string>{"mark", "1"}));
    EXPECT_EQ(rows->at(1).getValues(), (std::vector<std::string>{"ada", "2"}));
}

TEST_F(ExecutorTest, ExecuteWildcardProjectionReturnsWholeRow) {
    TableInfo* table_info = createUsersTable();
    ASSERT_NE(table_info, nullptr);

    std::optional<Tuple> tuple = RowCodec::serialize({"1", "mark", "mark@example.com"});
    ASSERT_TRUE(tuple.has_value());
    table_info->getTableHeap()->insertTuple(tuple.value());

    SelectStatement statement({ColumnExpression::wildcard()}, "users");
    std::optional<BoundTableStatement> bound = binder_.bindSelect(statement);
    ASSERT_TRUE(bound.has_value());

    std::optional<std::vector<ResultRow>> rows = executor_.execute(bound.value());

    ASSERT_TRUE(rows.has_value());
    ASSERT_EQ(rows->size(), 1);
    EXPECT_EQ(rows->at(0).getValues(), (std::vector<std::string>{"1", "mark", "mark@example.com"}));
}

TEST_F(ExecutorTest, ExecuteSkipsDeletedTuplesDuringScan) {
    TableInfo* table_info = createUsersTable();
    ASSERT_NE(table_info, nullptr);

    std::optional<Tuple> first = RowCodec::serialize({"1", "mark", "mark@example.com"});
    std::optional<Tuple> second = RowCodec::serialize({"2", "ada", "ada@example.com"});
    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());

    RID first_rid = table_info->getTableHeap()->insertTuple(first.value());
    table_info->getTableHeap()->insertTuple(second.value());
    ASSERT_TRUE(table_info->getTableHeap()->applyDelete(first_rid));

    SelectStatement statement({ColumnExpression("id")}, "users");
    std::optional<BoundTableStatement> bound = binder_.bindSelect(statement);
    ASSERT_TRUE(bound.has_value());

    std::optional<std::vector<ResultRow>> rows = executor_.execute(bound.value());

    ASSERT_TRUE(rows.has_value());
    ASSERT_EQ(rows->size(), 1);
    EXPECT_EQ(rows->at(0).getValues(), (std::vector<std::string>{"2"}));
}

TEST_F(ExecutorTest, ExecuteFailsWhenTupleCannotBeDecodedAgainstSchema) {
    TableInfo* table_info = createUsersTable();
    ASSERT_NE(table_info, nullptr);

    Tuple malformed;
    malformed.data_ = {'o', 'n', 'l', 'y', '_', 'o', 'n', 'e'};
    table_info->getTableHeap()->insertTuple(malformed);

    SelectStatement statement({ColumnExpression("id")}, "users");
    std::optional<BoundTableStatement> bound = binder_.bindSelect(statement);
    ASSERT_TRUE(bound.has_value());

    EXPECT_FALSE(executor_.execute(bound.value()).has_value());
}

TEST_F(ExecutorTest, InsertExecutorSerializesAndInsertsRowThatSelectCanReadBack) {
    TableInfo* table_info = createUsersTable();
    ASSERT_NE(table_info, nullptr);

    InsertStatement insert_statement("users", {"1", "mark", "mark@example.com"});
    std::optional<BoundInsertStatement> bound_insert = binder_.bindInsert(insert_statement);
    ASSERT_TRUE(bound_insert.has_value());

    std::optional<RID> rid = insert_executor_.execute(bound_insert.value());
    ASSERT_TRUE(rid.has_value());
    EXPECT_GE(rid->page_id, 0);
    EXPECT_GE(rid->slot_id, 0);

    SelectStatement select_statement({ColumnExpression("name"), ColumnExpression("email")}, "users");
    std::optional<BoundTableStatement> bound_select = binder_.bindSelect(select_statement);
    ASSERT_TRUE(bound_select.has_value());

    std::optional<std::vector<ResultRow>> rows = executor_.execute(bound_select.value());
    ASSERT_TRUE(rows.has_value());
    ASSERT_EQ(rows->size(), 1);
    EXPECT_EQ(rows->at(0).getValues(), (std::vector<std::string>{"mark", "mark@example.com"}));
}

TEST(ExecutorPersistenceTests, ReopenedTableHeapCanReadPreviouslyInsertedRows) {
    const std::string db_path = "/Users/markseeliger/CLionProjects/marksql/Storage/TestDBs/executor_persist.db";

    int first_page_id = -1;
    {
        DiskManager dm(db_path);
        BufferPoolManager bpm(&dm);
        Catalog catalog(&dm, &bpm);
        Binder binder(&catalog);
        InsertExecutor insert_executor;

        Schema schema({
            Column("id", TypeId::INTEGER, false),
            Column("name", TypeId::TEXT),
            Column("email", TypeId::TEXT)
        });

        TableInfo* table_info = catalog.createTable("users", schema);
        ASSERT_NE(table_info, nullptr);
        first_page_id = table_info->getTableHeap()->getFirstPageId();

        InsertStatement insert_statement("users", {"1", "mark", "mark@example.com"});
        std::optional<BoundInsertStatement> bound_insert = binder.bindInsert(insert_statement);
        ASSERT_TRUE(bound_insert.has_value());
        ASSERT_TRUE(insert_executor.execute(bound_insert.value()).has_value());
    }

    {
        DiskManager dm(db_path);
        BufferPoolManager bpm(&dm);
        Catalog catalog(&dm, &bpm);
        Binder binder(&catalog);
        SelectExecutor select_executor;

        Schema schema({
            Column("id", TypeId::INTEGER, false),
            Column("name", TypeId::TEXT),
            Column("email", TypeId::TEXT)
        });

        TableInfo* table_info = catalog.loadTable("users", schema, first_page_id);
        ASSERT_NE(table_info, nullptr);

        SelectStatement select_statement({ColumnExpression("name"), ColumnExpression("email")}, "users");
        std::optional<BoundTableStatement> bound_select = binder.bindSelect(select_statement);
        ASSERT_TRUE(bound_select.has_value());

        std::optional<std::vector<ResultRow>> rows = select_executor.execute(bound_select.value());
        ASSERT_TRUE(rows.has_value());
        ASSERT_EQ(rows->size(), 1);
        EXPECT_EQ(rows->at(0).getValues(), (std::vector<std::string>{"mark", "mark@example.com"}));
    }
}
