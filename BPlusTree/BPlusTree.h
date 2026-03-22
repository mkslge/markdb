
#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "InternalNode.h"
#include "LeafNode.h"
#include "Node.h"
#include <queue>
#include <vector>
#include <memory>


template <typename T>
class BPlusTree {
    private:

    std::unique_ptr<Node<T>> root_;
    int order_;


    public:
        BPlusTree(int order) : order_(order) {
            root_ = std::make_unique<LeafNode<T>>();
        }


        

        bool add(const T& val);
        bool remove(const T& val);
        bool contains(const T& val);

        std::vector<T> getRange(const T& lo, const T& hi);

        std::string toString();
        


    private:
        void splitNode(Node<T>* node, std::vector<Node<T>*>& ancestors);
        Node<T>* findChild(InternalNode<T> *node, const T& val);
        bool addHelper(Node<T>* node, const T& val, std::vector<Node<T>*>& ancestors);


        bool containsHelper(Node<T>* node, const T& val);

};


#include "BPlusTree.tpp"
#endif
