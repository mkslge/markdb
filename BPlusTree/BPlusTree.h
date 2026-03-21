
#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "InternalNode.h"
#include "LeafNode.h"
#include "Node.h"
#include <queue>
#include <vector>


template <typename T>
class BPlusTree {
    private:

    Node<T>* root_;
    int order_;


    public:
        BPlusTree(int order) : order_(order) {
            root_ = new LeafNode<T>();
        }

        bool add(const T& val);
        bool remove(const T& val);
        bool contains(const T& val);

        std::string toString();
        


    private:
        void splitNode(Node<T>* node);
        Node<T>* findChild(InternalNode<T> *node, const T& val);
        bool addHelper(Node<T>* node, const T& val);


        bool containsHelper(Node<T>* node, const T& val);

};


#include "BPlusTree.tpp"
#endif
