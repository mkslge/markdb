
#include "BPlusTree.h"
#include <memory>

template<typename T>
bool BPlusTree<T>::add(const T& val) {
    

    std::vector<Node<T>*> ancestors{};
    return this->addHelper(root_.get(), val, ancestors);
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

    std::unique_ptr<Node<T>> left_child;
    std::unique_ptr<Node<T>> right_child;
    T separator_key{};

    if (node->isLeaf()) {
        std::vector<T> left_keys(keys.begin(), keys.begin() + static_cast<long>(split_index));
        std::vector<T> right_keys(keys.begin() + static_cast<long>(split_index), keys.end());

        auto left_leaf = std::make_unique<LeafNode<T>>();
        auto right_leaf = std::make_unique<LeafNode<T>>();
        left_leaf->setKeys(left_keys);
        right_leaf->setKeys(right_keys);

        left_child = std::move(left_leaf);
        right_child = std::move(right_leaf);
        separator_key = right_keys.front();
    } else {
        auto* internal_node = static_cast<InternalNode<T>*>(node);
        std::vector<std::unique_ptr<Node<T>>> children = internal_node->takeChildren();

        std::vector<T> left_keys(keys.begin(), keys.begin() + static_cast<long>(split_index));
        std::vector<T> right_keys(keys.begin() + static_cast<long>(split_index + 1), keys.end());

        std::vector<std::unique_ptr<Node<T>>> left_children;
        std::vector<std::unique_ptr<Node<T>>> right_children;
        left_children.reserve(split_index + 1);
        right_children.reserve(children.size() - (split_index + 1));

        for (size_t i = 0; i < children.size(); i++) {
            if (i <= split_index) {
                left_children.push_back(std::move(children[i]));
            } else {
                right_children.push_back(std::move(children[i]));
            }
        }

        auto left_internal = std::make_unique<InternalNode<T>>();
        auto right_internal = std::make_unique<InternalNode<T>>();
        left_internal->setValues(left_keys);
        left_internal->setChildren(std::move(left_children));
        right_internal->setValues(right_keys);
        right_internal->setChildren(std::move(right_children));

        left_child = std::move(left_internal);
        right_child = std::move(right_internal);
        separator_key = keys[split_index];
    }

    if (ancestors.empty()) {
        auto new_root = std::make_unique<InternalNode<T>>();
        new_root->setValues({separator_key});
        new_root->insertChild(std::move(left_child), 0);
        new_root->insertChild(std::move(right_child), 1);
        root_ = std::move(new_root);
        return;
    }

    auto* parent = static_cast<InternalNode<T>*>(ancestors.back());
    const int child_index = parent->indexOfChild(node);
    assert(child_index >= 0);

    std::vector<T> parent_keys = parent->getList();
    parent_keys.insert(parent_keys.begin() + child_index, separator_key);
    parent->setValues(parent_keys);
    parent->replaceChildWithSplit(child_index, std::move(left_child), std::move(right_child));

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
    return containsHelper(root_.get(), val);
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
    q.push(root_.get());
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




