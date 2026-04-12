//
// Created by Mark on 4/12/26.
//

#include <gtest/gtest.h>
#include "../BinderLayer/Binder.h"
#include "../SyntaxLayer/InsertStatement.h"
#include "../SyntaxLayer/SelectStatement.h"

class BinderTest : public ::testing::Test {
protected:
    DiskManager dm_{"/Users/markseeliger/CLionProjects/marksql/Storage/TestDBs/binder.db"};
    BufferPoolManager bpm_{&dm_};
    Catalog catalog_{&dm_, &bpm_};
    Binder binder_{&catalog_};

    void SetUp() override {
        Schema schema({
            Column("id", TypeId::INTEGER, false),
            Column("name", TypeId::TEXT),
            Column("email", TypeId::TEXT)
        });
        ASSERT_NE(catalog_.createTable("users", schema), nullptr);
    }
};

TEST_F(BinderTest, BindTableColumnsResolvesNamedColumnsInRequestedOrder) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("users", {"name", "id"});

    ASSERT_TRUE(bound.has_value());
    ASSERT_NE(bound->getTableInfo(), nullptr);
    EXPECT_EQ(bound->getTableInfo()->getTableName(), "users");
    ASSERT_EQ(bound->getColumns().size(), 2);
    EXPECT_EQ(bound->getColumns()[0].getColumn()->getName(), "name");
    EXPECT_EQ(bound->getColumns()[0].getColumnIndex(), 1);
    EXPECT_EQ(bound->getColumns()[1].getColumn()->getName(), "id");
    EXPECT_EQ(bound->getColumns()[1].getColumnIndex(), 0);
}

TEST_F(BinderTest, BindSelectResolvesNamedColumnsFromAst) {
    SelectStatement statement({
        ColumnExpression("name"),
        ColumnExpression("id")
    }, "users");

    std::optional<BoundTableStatement> bound = binder_.bindSelect(statement);

    ASSERT_TRUE(bound.has_value());
    ASSERT_NE(bound->getTableInfo(), nullptr);
    EXPECT_EQ(bound->getTableInfo()->getTableName(), "users");
    ASSERT_EQ(bound->getColumns().size(), 2);
    EXPECT_EQ(bound->getColumns()[0].getColumn()->getName(), "name");
    EXPECT_EQ(bound->getColumns()[0].getColumnIndex(), 1);
    EXPECT_EQ(bound->getColumns()[1].getColumn()->getName(), "id");
    EXPECT_EQ(bound->getColumns()[1].getColumnIndex(), 0);
}

TEST_F(BinderTest, BindSelectExpandsWildcardFromAst) {
    SelectStatement statement({ColumnExpression::wildcard()}, "users");

    std::optional<BoundTableStatement> bound = binder_.bindSelect(statement);

    ASSERT_TRUE(bound.has_value());
    ASSERT_EQ(bound->getColumns().size(), 3);
    EXPECT_EQ(bound->getColumns()[0].getColumn()->getName(), "id");
    EXPECT_EQ(bound->getColumns()[1].getColumn()->getName(), "name");
    EXPECT_EQ(bound->getColumns()[2].getColumn()->getName(), "email");
}

TEST_F(BinderTest, BindInsertAcceptsValuesMatchingSchemaColumnCount) {
    InsertStatement statement("users", {"1", "mark", "mark@example.com"});

    std::optional<BoundInsertStatement> bound = binder_.bindInsert(statement);

    ASSERT_TRUE(bound.has_value());
    EXPECT_EQ(bound->getTableInfo()->getTableName(), "users");
    EXPECT_EQ(bound->getValues(), (std::vector<std::string>{"1", "mark", "mark@example.com"}));
}

TEST_F(BinderTest, BoundColumnsPointIntoResolvedTableSchema) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("users", {"name"});

    ASSERT_TRUE(bound.has_value());
    const Schema& schema = bound->getTableInfo()->getSchema();
    ASSERT_EQ(bound->getColumns().size(), 1);
    EXPECT_EQ(bound->getColumns()[0].getColumn(), &schema.getColumn(1));
}

TEST_F(BinderTest, BindTableColumnsExpandsWildcardToWholeSchema) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("users", {"*"});

    ASSERT_TRUE(bound.has_value());
    ASSERT_EQ(bound->getColumns().size(), 3);
    EXPECT_EQ(bound->getColumns()[0].getColumn()->getName(), "id");
    EXPECT_EQ(bound->getColumns()[1].getColumn()->getName(), "name");
    EXPECT_EQ(bound->getColumns()[2].getColumn()->getName(), "email");
}

TEST_F(BinderTest, BindTableColumnsTreatsEmptyColumnListAsWildcard) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("users", {});

    ASSERT_TRUE(bound.has_value());
    ASSERT_EQ(bound->getColumns().size(), 3);
}

TEST_F(BinderTest, BindTableColumnsRejectsUnknownTable) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("missing", {"id"});

    EXPECT_FALSE(bound.has_value());
}

TEST_F(BinderTest, BindSelectRejectsUnknownTableFromAst) {
    SelectStatement statement({ColumnExpression("id")}, "missing");

    EXPECT_FALSE(binder_.bindSelect(statement).has_value());
}

TEST_F(BinderTest, BindInsertRejectsUnknownTableFromAst) {
    InsertStatement statement("missing", {"1", "mark", "mark@example.com"});

    EXPECT_FALSE(binder_.bindInsert(statement).has_value());
}

TEST_F(BinderTest, BindTableColumnsRejectsUnknownColumn) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("users", {"age"});

    EXPECT_FALSE(bound.has_value());
}

TEST_F(BinderTest, BindTableColumnsRejectsEmptyTableName) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("", {"id"});

    EXPECT_FALSE(bound.has_value());
}

TEST_F(BinderTest, BindTableColumnsRejectsEmptyColumnName) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("users", {""});

    EXPECT_FALSE(bound.has_value());
}

TEST_F(BinderTest, BindInsertRejectsWrongValueCountForSchema) {
    InsertStatement too_few("users", {"1", "mark"});
    InsertStatement too_many("users", {"1", "mark", "mark@example.com", "extra"});

    EXPECT_FALSE(binder_.bindInsert(too_few).has_value());
    EXPECT_FALSE(binder_.bindInsert(too_many).has_value());
}

TEST_F(BinderTest, BindTableColumnsRejectsMixedWildcardAndNamedColumns) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("users", {"id", "*"});

    EXPECT_FALSE(bound.has_value());
}

TEST_F(BinderTest, BindTableColumnsRejectsWildcardWhenItIsNotTheOnlyProjection) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("users", {"*", "id"});

    EXPECT_FALSE(bound.has_value());
}

TEST_F(BinderTest, BindSelectRejectsWildcardWhenItIsNotTheOnlyProjection) {
    SelectStatement statement({
        ColumnExpression::wildcard(),
        ColumnExpression("id")
    }, "users");

    EXPECT_FALSE(binder_.bindSelect(statement).has_value());
}

TEST_F(BinderTest, BindTableColumnsAllowsDuplicateProjectedColumns) {
    std::optional<BoundTableStatement> bound = binder_.bindTableColumns("users", {"id", "id"});

    ASSERT_TRUE(bound.has_value());
    ASSERT_EQ(bound->getColumns().size(), 2);
    EXPECT_EQ(bound->getColumns()[0].getColumnIndex(), 0);
    EXPECT_EQ(bound->getColumns()[1].getColumnIndex(), 0);
}

TEST(BinderEdgeCaseTests, NullCatalogBinderRejectsAllBindings) {
    Binder binder(nullptr);
    SelectStatement statement({ColumnExpression("id")}, "users");
    InsertStatement insert_statement("users", {"1"});

    EXPECT_FALSE(binder.bindTableColumns("users", {"id"}).has_value());
    EXPECT_FALSE(binder.bindTableColumns("users", {"*"}).has_value());
    EXPECT_FALSE(binder.bindSelect(statement).has_value());
    EXPECT_FALSE(binder.bindInsert(insert_statement).has_value());
}
