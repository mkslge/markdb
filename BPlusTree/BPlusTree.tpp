
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
    std::vector<T> keys = node->getList();
    int size = node->size();
    std::vector<T> half1(keys.begin(), keys.begin() + size / 2);
    std::vector<T> half2(keys.begin() + (size / 2), keys.begin() + size );
    for(int elem : half1) {
        std::cout << elem << " ";
    }
    std::cout << "\n";
    for(int elem : half2) {
        std::cout << elem << " ";
    }

    T new_split = half2[0];
    




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


template<typename T>
std::string BPlusTree<T>::toString() {
    std::string builder = "";
    std::queue<Node<T>*> q;
    q.push(root_);
    while(!q.empty()) {
        size_t q_size = q.size();
        for(size_t i{}; i < q_size;i++) {
            Node<T>* curr_node = q.front();
            q.pop();
            builder.append(curr_node->toString());

            if(!curr_node->isLeaf()) {
                InternalNode<T>* curr_inode = static_cast<InternalNode<T>*>(curr_node);
                for(size_t i{}; i < curr_inode->children_size();i++) {
                    q.push(curr_inode->childAt(i));
                }

            }
        }
        builder.append("\n");
    }

    
    return builder;
}







