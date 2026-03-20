
#include "BPlusTree.h"

template<typename T>
bool BPlusTree<T>::add(const T& val) {
    


    return this->addHelper(root_, val);
}


template<typename T>
bool BPlusTree<T>::addHelper(Node<T>* node, const T& val) {
    assert(node != nullptr);

    if(node->isLeaf()) {
        node->add(val);
        if(node->size() == order_) {
            this->splitNode(node);
        }
        return true;
    } else {
        InternalNode<T> *internal_node = static_cast<InternalNode<T>*>(node);
        return this->addHelper(findChild(internal_node, val), val);
    }

    return true;
}


template<typename T>
void BPlusTree<T>::splitNode(Node<T>* node) {

}

template<typename T>
Node<T>* BPlusTree<T>::findChild(InternalNode<T>* node, const T& val) {

    for(size_t i{}; i < node->size();i++) {
        if(node->at(i) <= val) {
            return node->childAt(i);
        }
    }

    return node->childAt(node->children_size() - 1);
}


template<typename T>
bool BPlusTree<T>::contains(const T& val) {
    return containsHelper(root_, val);
}

template<typename T>
bool BPlusTree<T>::containsHelper(Node<T>* node, const T& val) {
    if(node == nullptr ) {
        return false;
    } else if(node->isLeaf()) {
        return node->contains(val);
    }

    InternalNode<T> *internal_node = static_cast<InternalNode<T>*>(node);
    return containsHelper(findChild(internal_node, val), val);
}