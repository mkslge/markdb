#include <gtest/gtest.h>
#include <vector>
#include "../BPlusTree/BPlusTree.h"

namespace {

void InsertAll(BPlusTree<int>& tree, const std::vector<int>& values) {
    for (int value : values) {
        EXPECT_TRUE(tree.add(value));
    }
}

}

TEST(BPlusTreeTests, EmptyTreeDoesNotContainValues) {
    BPlusTree<int> tree(3);

    EXPECT_FALSE(tree.contains(-1));
    EXPECT_FALSE(tree.contains(0));
    EXPECT_FALSE(tree.contains(42));
    EXPECT_EQ(tree.toString(), "\n");
}

TEST(BPlusTreeTests, InsertWithoutSplitKeepsLeafSorted) {
    BPlusTree<int> tree(5);
    InsertAll(tree, {4, 1, 3, 2});

    EXPECT_TRUE(tree.contains(1));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(4));
    EXPECT_FALSE(tree.contains(5));
    EXPECT_EQ(tree.toString(), "1 2 3 4 \n");
}

TEST(BPlusTreeTests, RootLeafSplitProducesExpectedTwoLevelShape) {
    BPlusTree<int> tree(3);
    InsertAll(tree, {1, 2, 3});

    EXPECT_TRUE(tree.contains(1));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_FALSE(tree.contains(0));
    EXPECT_FALSE(tree.contains(4));
    EXPECT_EQ(tree.toString(), "2 \n1 2 3 \n");
}

TEST(BPlusTreeTests, ContainsFindsSeparatorValuesAfterRootSplit) {
    BPlusTree<int> tree(3);
    InsertAll(tree, {10, 20, 30});

    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(30));
    EXPECT_FALSE(tree.contains(9));
    EXPECT_FALSE(tree.contains(21));
    EXPECT_FALSE(tree.contains(31));
}

TEST(BPlusTreeTests, SequentialInsertsCauseCascadingSplits) {
    BPlusTree<int> tree(3);
    InsertAll(tree, {1, 2, 3, 4, 5, 6, 7, 8});

    for (int value = 1; value <= 8; value++) {
        EXPECT_TRUE(tree.contains(value)) << "Missing value " << value;
    }
    EXPECT_FALSE(tree.contains(-5));
    EXPECT_FALSE(tree.contains(9));
    EXPECT_EQ(tree.toString(), "3 5 \n2 4 6 7 \n1 2 3 4 5 6 7 8 \n");
}

TEST(BPlusTreeTests, ContainsWorksAcrossAllLeafRangesAfterMultipleSplits) {
    BPlusTree<int> tree(3);
    InsertAll(tree, {1, 2, 3, 4, 5, 6, 7, 8});

    EXPECT_TRUE(tree.contains(1));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(8));
    EXPECT_FALSE(tree.contains(-1));
    EXPECT_FALSE(tree.contains(0));
    EXPECT_FALSE(tree.contains(10));
    EXPECT_FALSE(tree.contains(99));
}

TEST(BPlusTreeTests, MixedInsertionOrderStillBuildsOrderedLeaves) {
    BPlusTree<int> tree(4);
    InsertAll(tree, {8, 3, 7, 1, 6, 2, 5, 4});

    for (int value = 1; value <= 8; value++) {
        EXPECT_TRUE(tree.contains(value)) << "Missing value " << value;
    }
    EXPECT_FALSE(tree.contains(0));
    EXPECT_FALSE(tree.contains(10));
    EXPECT_EQ(tree.toString(), "3 5 7 \n1 2 3 4 5 6 7 8 \n");
}

TEST(BPlusTreeTests, DuplicateKeysAreRetainedAndSearchable) {
    BPlusTree<int> tree(3);
    InsertAll(tree, {7, 7, 7});

    EXPECT_TRUE(tree.contains(7));
    EXPECT_FALSE(tree.contains(6));
    EXPECT_FALSE(tree.contains(8));
    EXPECT_EQ(tree.toString(), "7 \n7 7 7 \n");
}

TEST(BPlusTreeTests, SplitPromotesExpectedSeparatorsForOrderFourTree) {
    BPlusTree<int> tree(4);
    InsertAll(tree, {1, 2, 3, 4, 5, 6, 7, 8});

    EXPECT_EQ(tree.toString(), "3 5 7 \n1 2 3 4 5 6 7 8 \n");
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(7));
}

TEST(BPlusTreeTests, GetRangeFromEmptyTreeReturnsEmptyVector) {
    BPlusTree<int> tree(3);

    EXPECT_EQ(tree.getRange(1, 5), std::vector<int>{});
}

TEST(BPlusTreeTests, GetRangeReturnsExactBoundaryMatches) {
    BPlusTree<int> tree(4);
    InsertAll(tree, {1, 2, 3, 4, 5, 6});

    EXPECT_EQ(tree.getRange(2, 5), (std::vector<int>{2, 3, 4, 5}));
}

TEST(BPlusTreeTests, GetRangeReturnsOnlyInteriorValuesAcrossSplits) {
    BPlusTree<int> tree(3);
    InsertAll(tree, {1, 2, 3, 4, 5, 6, 7, 8});

    EXPECT_EQ(tree.getRange(3, 6), (std::vector<int>{3, 4, 5, 6}));
}

TEST(BPlusTreeTests, GetRangeReturnsWholeTreeWhenBoundsCoverAllValues) {
    BPlusTree<int> tree(3);
    InsertAll(tree, {1, 2, 3, 4, 5, 6, 7, 8});

    EXPECT_EQ(tree.getRange(-10, 99), (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8}));
}

TEST(BPlusTreeTests, GetRangeReturnsEmptyWhenNoKeysFallInsideBounds) {
    BPlusTree<int> tree(4);
    InsertAll(tree, {10, 20, 30, 40});

    EXPECT_EQ(tree.getRange(21, 29), std::vector<int>{});
}

TEST(BPlusTreeTests, GetRangePreservesDuplicatesWithinBounds) {
    BPlusTree<int> tree(3);
    InsertAll(tree, {1, 2, 2, 2, 3, 4, 5});

    EXPECT_EQ(tree.getRange(2, 3), (std::vector<int>{2, 2, 2, 3}));
}
