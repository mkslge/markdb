
#include "BPlusTree.h"

template<typename T>
bool BPlusTree<T>::add(const T& val) {
    

    std::vector<Node<T>*> ancestors{};
    return this->addHelper(root_, val, ancestors);
}



template<typename T>
bool BPlusTree<T>::addHelper(Node<T>* node, const T& val, std::vector<Node<T>*>& ancestors) {
    assert(node != nullptr);

    if (node->isLeaf()) {
        node->add(val);
        if (node->size() == order_) {
            this->splitNode(node, ancestors);
        }
        return true;
    } 
    
    InternalNode<T>* internal_node = static_cast<InternalNode<T>*>(node);
    ancestors.push_back(node);
    return this->addHelper(findChild(internal_node, val), val, ancestors);
}


template<typename T>
void BPlusTree<T>::splitNode(Node<T>* node, std::vector<Node<T>*>& ancestors) {
    const std::vector<T> keys = node->getList();
    const size_t split_index = keys.size() / 2;

    Node<T>* left_child = nullptr;
    Node<T>* right_child = nullptr;
    T separator_key{};

    if (node->isLeaf()) {
        std::vector<T> left_keys(keys.begin(), keys.begin() + static_cast<long>(split_index));
        std::vector<T> right_keys(keys.begin() + static_cast<long>(split_index), keys.end());

        auto* left_leaf = new LeafNode<T>();
        auto* right_leaf = new LeafNode<T>();
        left_leaf->setKeys(left_keys);
        right_leaf->setKeys(right_keys);

        left_child = left_leaf;
        right_child = right_leaf;
        separator_key = right_keys.front();
    } else {
        auto* internal_node = static_cast<InternalNode<T>*>(node);
        const std::vector<Node<T>*> children = internal_node->children();

        std::vector<T> left_keys(keys.begin(), keys.begin() + static_cast<long>(split_index));
        std::vector<T> right_keys(keys.begin() + static_cast<long>(split_index + 1), keys.end());

        std::vector<Node<T>*> left_children(
                children.begin(),
                children.begin() + static_cast<long>(split_index + 1));
        std::vector<Node<T>*> right_children(
                children.begin() + static_cast<long>(split_index + 1),
                children.end());

        auto* left_internal = new InternalNode<T>();
        auto* right_internal = new InternalNode<T>();
        left_internal->setValues(left_keys);
        left_internal->setChildren(left_children);
        right_internal->setValues(right_keys);
        right_internal->setChildren(right_children);

        left_child = left_internal;
        right_child = right_internal;
        separator_key = keys[split_index];
    }

    if (ancestors.empty()) {
        auto* new_root = new InternalNode<T>();
        new_root->setValues({separator_key});
        new_root->setChildren({left_child, right_child});
        root_ = new_root;
        return;
    }

    auto* parent = static_cast<InternalNode<T>*>(ancestors.back());
    const int child_index = parent->indexOfChild(node);
    assert(child_index >= 0);

    std::vector<T> parent_keys = parent->getList();
    parent_keys.insert(parent_keys.begin() + child_index, separator_key);
    parent->setValues(parent_keys);
    parent->replaceChildWithSplit(child_index, left_child, right_child);

    if (parent->size() == order_) {
        ancestors.pop_back();
        splitNode(parent, ancestors);
    }
}

template<typename T>
Node<T>* BPlusTree<T>::findChild(InternalNode<T>* node, const T& val) {

    for(size_t i{}; i < node->size();i++) {
        if(val < node->at(i)) {
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





