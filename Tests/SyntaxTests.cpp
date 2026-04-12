//
// Created by Mark on 4/12/26.
//

#include <gtest/gtest.h>
#include "../SyntaxLayer/SelectStatement.h"

TEST(ColumnExpressionTests, NamedColumnExpressionStoresNameAndIsNotWildcard) {
    ColumnExpression column("id");

    EXPECT_EQ(column.getColumnName(), "id");
    EXPECT_FALSE(column.isWildcard());
}

TEST(ColumnExpressionTests, WildcardFactoryBuildsWildcardExpression) {
    ColumnExpression wildcard = ColumnExpression::wildcard();

    EXPECT_EQ(wildcard.getColumnName(), "*");
    EXPECT_TRUE(wildcard.isWildcard());
}

TEST(SelectStatementTests, SelectStatementPreservesProjectionOrderAndTableName) {
    SelectStatement statement({
        ColumnExpression("name"),
        ColumnExpression("id")
    }, "users");

    EXPECT_EQ(statement.getTableName(), "users");
    ASSERT_EQ(statement.getColumns().size(), 2);
    EXPECT_EQ(statement.getColumns()[0].getColumnName(), "name");
    EXPECT_EQ(statement.getColumns()[1].getColumnName(), "id");
    EXPECT_FALSE(statement.isWildcardSelect());
}

TEST(SelectStatementTests, WildcardSelectIsRecognized) {
    SelectStatement statement({ColumnExpression::wildcard()}, "users");

    EXPECT_EQ(statement.getTableName(), "users");
    ASSERT_EQ(statement.getColumns().size(), 1);
    EXPECT_TRUE(statement.getColumns()[0].isWildcard());
    EXPECT_TRUE(statement.isWildcardSelect());
}

TEST(SelectStatementTests, MixedProjectionIsNotTreatedAsWildcardSelect) {
    SelectStatement statement({
        ColumnExpression::wildcard(),
        ColumnExpression("id")
    }, "users");

    EXPECT_FALSE(statement.isWildcardSelect());
}
