//
// Created by Mark on 4/12/26.
//

#include <gtest/gtest.h>
#include <algorithm>
#include "../CatalogLayer/Catalog.h"

class CatalogTest : public ::testing::Test {
protected:
    DiskManager dm_{"/Users/markseeliger/CLionProjects/marksql/Storage/TestDBs/catalog.db"};
    BufferPoolManager bpm_{&dm_};
};

TEST(SchemaTests, SchemaPreservesColumnOrderAndSupportsNameLookup) {
    Schema schema({
        Column("id", TypeId::INTEGER, false),
        Column("name", TypeId::TEXT),
        Column("email", TypeId::TEXT)
    });

    ASSERT_EQ(schema.getColumnCount(), 3);
    EXPECT_EQ(schema.getColumn(0).getName(), "id");
    EXPECT_EQ(schema.getColumn(1).getName(), "name");
    EXPECT_EQ(schema.getColumn(2).getName(), "email");
    EXPECT_EQ(schema.getColumn(0).getType(), TypeId::INTEGER);
    EXPECT_FALSE(schema.getColumn(0).isNullable());
    EXPECT_TRUE(schema.hasColumn("name"));
    EXPECT_EQ(schema.getColumnIndex("email"), 2);
    EXPECT_EQ(schema.getColumnIndex("missing"), std::nullopt);
}

TEST(SchemaTests, GetColumnThrowsWhenIndexIsOutOfRange) {
    Schema schema({
        Column("id", TypeId::INTEGER, false)
    });

    EXPECT_THROW(static_cast<void>(schema.getColumn(1)), std::out_of_range);
}

TEST(SchemaTests, EmptySchemaReportsNoColumns) {
    Schema schema;

    EXPECT_EQ(schema.getColumnCount(), 0);
    EXPECT_FALSE(schema.hasColumn("id"));
    EXPECT_EQ(schema.getColumnIndex("id"), std::nullopt);
}

TEST_F(CatalogTest, CreateTableStoresSchemaAndAllocatesTableHeap) {
    Catalog catalog(&dm_, &bpm_);
    Schema schema({
        Column("id", TypeId::INTEGER, false),
        Column("name", TypeId::TEXT)
    });

    TableInfo* table_info = catalog.createTable("users", schema);

    ASSERT_NE(table_info, nullptr);
    EXPECT_TRUE(catalog.hasTable("users"));
    EXPECT_EQ(catalog.getTable("users"), table_info);
    EXPECT_EQ(table_info->getTableName(), "users");
    ASSERT_NE(table_info->getTableHeap(), nullptr);
    ASSERT_EQ(table_info->getSchema().getColumnCount(), 2);
    EXPECT_EQ(table_info->getSchema().getColumn(0).getName(), "id");
    EXPECT_EQ(table_info->getSchema().getColumn(1).getName(), "name");
}

TEST_F(CatalogTest, DuplicateTableNameIsRejected) {
    Catalog catalog(&dm_, &bpm_);
    Schema schema({Column("id", TypeId::INTEGER, false)});

    ASSERT_NE(catalog.createTable("users", schema), nullptr);
    EXPECT_EQ(catalog.createTable("users", schema), nullptr);
}

TEST_F(CatalogTest, DuplicateColumnNamesAreRejected) {
    Catalog catalog(&dm_, &bpm_);
    Schema bad_schema({
        Column("id", TypeId::INTEGER, false),
        Column("id", TypeId::TEXT)
    });

    EXPECT_EQ(catalog.createTable("users", bad_schema), nullptr);
    EXPECT_FALSE(catalog.hasTable("users"));
}

TEST_F(CatalogTest, EmptyTableNameIsRejected) {
    Catalog catalog(&dm_, &bpm_);

    EXPECT_EQ(catalog.createTable("", Schema({Column("id", TypeId::INTEGER, false)})), nullptr);
}

TEST_F(CatalogTest, EmptySchemaIsRejected) {
    Catalog catalog(&dm_, &bpm_);

    EXPECT_EQ(catalog.createTable("users", Schema{}), nullptr);
    EXPECT_FALSE(catalog.hasTable("users"));
}

TEST_F(CatalogTest, EmptyColumnNameIsRejected) {
    Catalog catalog(&dm_, &bpm_);
    Schema bad_schema({
        Column("", TypeId::INTEGER, false),
        Column("name", TypeId::TEXT)
    });

    EXPECT_EQ(catalog.createTable("users", bad_schema), nullptr);
    EXPECT_FALSE(catalog.hasTable("users"));
}

TEST_F(CatalogTest, GetTableReturnsNullptrWhenTableDoesNotExist) {
    Catalog catalog(&dm_, &bpm_);

    EXPECT_EQ(catalog.getTable("missing"), nullptr);
}

TEST_F(CatalogTest, HasTableReturnsFalseWhenTableDoesNotExist) {
    Catalog catalog(&dm_, &bpm_);

    EXPECT_FALSE(catalog.hasTable("missing"));
}

TEST_F(CatalogTest, ListTablesReturnsCreatedTableNames) {
    Catalog catalog(&dm_, &bpm_);

    ASSERT_NE(catalog.createTable("users", Schema({Column("id", TypeId::INTEGER, false)})), nullptr);
    ASSERT_NE(catalog.createTable("orders", Schema({Column("order_id", TypeId::INTEGER, false)})), nullptr);

    std::vector<std::string> table_names = catalog.listTables();

    EXPECT_EQ(table_names.size(), 2);
    EXPECT_NE(std::find(table_names.begin(), table_names.end(), "users"), table_names.end());
    EXPECT_NE(std::find(table_names.begin(), table_names.end(), "orders"), table_names.end());
}

TEST_F(CatalogTest, ListTablesStartsEmpty) {
    Catalog catalog(&dm_, &bpm_);

    EXPECT_TRUE(catalog.listTables().empty());
}
