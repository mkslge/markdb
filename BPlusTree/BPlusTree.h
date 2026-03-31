#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "InternalNode.h"
#include "LeafNode.h"
#include "Node.h"
#include <cmath>
#include <memory>
#include <queue>
#include <vector>

template <typename T>
class BPlusTree {
private:
    std::unique_ptr<Node<T>> root_;
    int order_;

public:
    BPlusTree(int order) : root_(std::make_unique<LeafNode<T>>()), order_(order) {}

    /*
     * Preconditions:
     * - `order_` is a valid B+ tree order greater than 1.
     *
     * Postconditions:
     * - `val` is inserted into the tree.
     * - The tree remains a valid B+ tree after any required splits.
     */
    bool add(const T& val);

    /*
     * Preconditions:
     * - None.
     *
     * Postconditions:
     * - Removes one occurrence of `val` if present.
     * - Returns `true` if a value was deleted, otherwise `false`.
     * - The tree is normalized before returning.
     */
    bool remove(const T& val);

    bool contains(const T& val);
    std::vector<T> getRange(const T& lo, const T& hi);
    std::string toString();

    int getMinNodes() {
        return static_cast<int>(std::ceil(order_ / 2.0) - 1);
    }

    int getMaxNodes() {
        return order_ - 1;
    }

private:
    /*
     * Preconditions:
     * - `node` currently contains `order_` keys.
     * - `ancestors` contains the path of internal parents from root to `node`.
     *
     * Postconditions:
     * - `node` is replaced by two valid nodes.
     * - The promoted separator is inserted into the parent, splitting upward if needed.
     */
    void splitNode(Node<T>* node, std::vector<Node<T>*>& ancestors);

    Node<T>* findChild(InternalNode<T>* node, const T& val);
    bool addHelper(Node<T>* node, const T& val, std::vector<Node<T>*>& ancestors);
    bool containsHelper(Node<T>* node, const T& val);
    bool removeHelper(const T& val, Node<T>* node, Node<T>* parent);

    /*
     * Preconditions:
     * - `leaf` has already had one key removed.
     * - `parent` is the structural parent of `leaf`.
     *
     * Postconditions:
     * - `leaf` is repaired by borrowing or merging.
     * - Parent separators and leaf links are updated to remain valid.
     */
    void rebalanceLeaf(LeafNode<T>* leaf, InternalNode<T>* parent, const T& val);

    void collapseRootIfNeeded();
    T refreshSeparators(Node<T>* node);
    void rebuildTreeFromLeaves();
};

#include "BPlusTree.tpp"
#endif
