#include <gtest/gtest.h>
#include "../BPlusTree/BPlusTree.h"
#include "../BPlusTree/InternalNode.h"
#include "../BPlusTree/LeafNode.h"
#include "../BPlusTree/Node.h"


TEST(BPlusTreeTests, VeryBasicTest) {
    BPlusTree<int> tree(5);
    tree.add(1);
    tree.add(2);
    tree.add(3);

    EXPECT_TRUE(tree.contains(1));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_EQ(tree.toString(), "1 2 3 \n");
}



TEST(BPlusTreeTests, BasicSplittingTest) {
    BPlusTree<int> tree(3);
    tree.add(1);
    tree.add(2);
    tree.add(3);

    

    EXPECT_TRUE(tree.contains(1));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(3));
    
    EXPECT_EQ(tree.toString(), "2\n1 2 3");
}

