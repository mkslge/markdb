#include "BPlusTree.h"
#include <functional>
#include <memory>

template <typename T>
bool BPlusTree<T>::add(const T& val) {
    std::vector<Node<T>*> ancestors;
    return addHelper(root_.get(), val, ancestors);
}

template <typename T>
bool BPlusTree<T>::addHelper(Node<T>* node, const T& val, std::vector<Node<T>*>& ancestors) {
    assert(node != nullptr);

    if (node->isLeaf()) {
        node->add(val);
        if (node->size() == static_cast<std::size_t>(order_)) {
            splitNode(node, ancestors);
        }
        return true;
    }

    auto* internal_node = static_cast<InternalNode<T>*>(node);
    ancestors.push_back(node);
    return addHelper(findChild(internal_node, val), val, ancestors);
}

template <typename T>
void BPlusTree<T>::splitNode(Node<T>* node, std::vector<Node<T>*>& ancestors) {
    const std::vector<T> keys = node->getList();
    const std::size_t split_index = keys.size() / 2;

    std::unique_ptr<Node<T>> left_child;
    std::unique_ptr<Node<T>> right_child;
    T separator_key{};

    if (node->isLeaf()) {
        auto* old_leaf = static_cast<LeafNode<T>*>(node);
        std::vector<T> left_keys(keys.begin(), keys.begin() + static_cast<long>(split_index));
        std::vector<T> right_keys(keys.begin() + static_cast<long>(split_index), keys.end());

        auto left_leaf = std::make_unique<LeafNode<T>>();
        auto right_leaf = std::make_unique<LeafNode<T>>();
        left_leaf->setKeys(left_keys);
        right_leaf->setKeys(right_keys);

        // Leaf nodes form the range-scan chain, so splitting a leaf must splice
        // the two replacement leaves into that doubly-linked list.
        LeafNode<T>* old_prev = old_leaf->prev();
        LeafNode<T>* old_next = old_leaf->next();
        LeafNode<T>* left_leaf_raw = left_leaf.get();
        LeafNode<T>* right_leaf_raw = right_leaf.get();

        left_leaf->setPrev(old_prev);
        left_leaf->setNext(right_leaf_raw);
        right_leaf->setPrev(left_leaf_raw);
        right_leaf->setNext(old_next);

        if (old_prev != nullptr) {
            old_prev->setNext(left_leaf_raw);
        }
        if (old_next != nullptr) {
            old_next->setPrev(right_leaf_raw);
        }

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

        for (std::size_t i = 0; i < children.size(); i++) {
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

    if (parent->size() == static_cast<std::size_t>(order_)) {
        ancestors.pop_back();
        splitNode(parent, ancestors);
    }
}

template <typename T>
Node<T>* BPlusTree<T>::findChild(InternalNode<T>* node, const T& val) {
    for (std::size_t i{}; i < node->size(); i++) {
        if (val < node->at(static_cast<int>(i))) {
            return node->childAt(static_cast<int>(i));
        }
    }

    return node->childAt(static_cast<int>(node->children_size() - 1));
}

template <typename T>
bool BPlusTree<T>::contains(const T& val) {
    return containsHelper(root_.get(), val);
}

template <typename T>
bool BPlusTree<T>::containsHelper(Node<T>* node, const T& val) {
    if (node == nullptr) {
        return false;
    }
    if (node->isLeaf()) {
        return node->contains(val);
    }

    auto* internal_node = static_cast<InternalNode<T>*>(node);
    return containsHelper(findChild(internal_node, val), val);
}

template <typename T>
bool BPlusTree<T>::remove(const T& val) {
    if (!removeHelper(val, root_.get(), nullptr)) {
        return false;
    }

    // Deletion may leave a redundant root or stale separators, so normalize
    // those invariants before returning control to the caller.
    collapseRootIfNeeded();

    if (root_ == nullptr) {
        root_ = std::make_unique<LeafNode<T>>();
        return true;
    }

    if (!root_->isLeaf()) {
        refreshSeparators(root_.get());
        collapseRootIfNeeded();
    }

    return true;
}

template <typename T>
bool BPlusTree<T>::removeHelper(const T& val, Node<T>* node, Node<T>* parent) {
    if (node == nullptr) {
        return false;
    }

    if (node->isLeaf()) {
        auto* leaf_node = static_cast<LeafNode<T>*>(node);
        if (!leaf_node->remove(val)) {
            return false;
        }

        if (leaf_node == root_.get()) {
            return true;
        }

        auto* parent_node = static_cast<InternalNode<T>*>(parent);
        const int child_index = parent_node->indexOfChild(leaf_node);
        assert(child_index >= 0);

        // If the leaf still satisfies minimum occupancy, only the separator that
        // points to this leaf might need to change.
        if (leaf_node->size() >= static_cast<std::size_t>(getMinNodes())) {
            if (child_index > 0 && leaf_node->size() > 0) {
                std::vector<T> parent_keys = parent_node->getList();
                parent_keys[child_index - 1] = leaf_node->at(0);
                parent_node->setValues(parent_keys);
            }
            return true;
        }

        rebalanceLeaf(leaf_node, parent_node, val);
        return true;
    }

    auto* internal_node = static_cast<InternalNode<T>*>(node);
    return removeHelper(val, findChild(internal_node, val), node);
}

template <typename T>
void BPlusTree<T>::rebalanceLeaf(LeafNode<T>* leaf, InternalNode<T>* parent, const T& val) {
    (void)val;

    const int min_size = getMinNodes();
    const int child_index = parent->indexOfChild(leaf);
    assert(child_index >= 0);

    //grabbing children only if index is in bounds.
    LeafNode<T>* left_sibling =
            child_index > 0 ? static_cast<LeafNode<T>*>(parent->childAt(child_index - 1)) : nullptr;
    LeafNode<T>* right_sibling =
            child_index + 1 < static_cast<int>(parent->children_size())
                    ? static_cast<LeafNode<T>*>(parent->childAt(child_index + 1))
                    : nullptr;


    if (right_sibling != nullptr && right_sibling->size() > static_cast<std::size_t>(min_size)) {
        //grabbing smallest right child
        const T borrowed = right_sibling->at(0);
        right_sibling->remove(borrowed);
        leaf->add(borrowed);

        //adjusting parent seperator
        std::vector<T> parent_keys = parent->getList();
        parent_keys[child_index] = right_sibling->at(0);
        parent->setValues(parent_keys);
        return;
    }

    if (left_sibling != nullptr && left_sibling->size() > static_cast<std::size_t>(min_size)) {
        //grabbing largest left child
        const T borrowed = left_sibling->at(static_cast<int>(left_sibling->size() - 1));
        left_sibling->remove(borrowed);
        leaf->add(borrowed);

        //adjusting parent seperator
        std::vector<T> parent_keys = parent->getList();
        parent_keys[child_index - 1] = leaf->at(0);
        parent->setValues(parent_keys);
        return;
    }

    std::vector<T> parent_keys = parent->getList();
    std::vector<std::unique_ptr<Node<T>>> children = parent->takeChildren();

    //if we cant steal from our siblings we have to merge
    if (left_sibling != nullptr) {
        std::vector<T> merged_keys = left_sibling->getList();
        std::vector<T> leaf_keys = leaf->getList();
        merged_keys.insert(merged_keys.end(), leaf_keys.begin(), leaf_keys.end());
        left_sibling->setKeys(merged_keys);
        left_sibling->setNext(leaf->next());
        if (leaf->next() != nullptr) {
            leaf->next()->setPrev(left_sibling);
        }

        parent_keys.erase(parent_keys.begin() + (child_index - 1));
        children.erase(children.begin() + child_index);
    } else if (right_sibling != nullptr) {
        std::vector<T> merged_keys = leaf->getList();
        std::vector<T> right_keys = right_sibling->getList();
        merged_keys.insert(merged_keys.end(), right_keys.begin(), right_keys.end());
        leaf->setKeys(merged_keys);

        leaf->setNext(right_sibling->next());
        if (right_sibling->next() != nullptr) {
            right_sibling->next()->setPrev(leaf);
        }

        parent_keys.erase(parent_keys.begin() + child_index);
        children.erase(children.begin() + (child_index + 1));
    } else {
        return;
    }

    //check to reset root
    if (parent == root_.get() && parent_keys.empty()) {
        root_ = std::move(children.front());
        return;
    }


    parent->setValues(parent_keys);
    parent->setChildren(std::move(children));

    if (parent == root_.get() || parent->size() >= static_cast<std::size_t>(getMinNodes())) {
        return;
    }

    //internal delete rebalance is still delegated to a rebuild. keeps leaf
    //repair small and lets the upper levels be reconstructed consistently.
    rebuildTreeFromLeaves();
}

template <typename T>
void BPlusTree<T>::collapseRootIfNeeded() {
    while (root_ != nullptr && !root_->isLeaf()) {
        auto* root_internal = static_cast<InternalNode<T>*>(root_.get());
        if (root_internal->children_size() == 0) {
            root_ = std::make_unique<LeafNode<T>>();
            return;
        }
        if (root_internal->children_size() != 1) {
            return;
        }

        std::vector<std::unique_ptr<Node<T>>> only_child = root_internal->takeChildren();
        root_ = std::move(only_child.front());
    }
}

template <typename T>
T BPlusTree<T>::refreshSeparators(Node<T>* node) {
    if (node->isLeaf()) {
        return node->at(0);
    }

    auto* internal_node = static_cast<InternalNode<T>*>(node);
    std::vector<T> new_keys;
    T first_key{};
    bool have_first = false;

    // Every separator is the first key in its right child.
    for (std::size_t i{}; i < internal_node->children_size(); i++) {
        T child_first = refreshSeparators(internal_node->childAt(static_cast<int>(i)));
        if (!have_first) {
            first_key = child_first;
            have_first = true;
        } else {
            new_keys.push_back(child_first);
        }
    }

    internal_node->setValues(new_keys);
    return first_key;
}

template <typename T>
void BPlusTree<T>::rebuildTreeFromLeaves() {
    std::vector<T> keys;

    std::function<void(Node<T>*)> collect_keys = [&](Node<T>* node) {
        if (node == nullptr) {
            return;
        }
        if (node->isLeaf()) {
            std::vector<T> leaf_keys = node->getList();
            keys.insert(keys.end(), leaf_keys.begin(), leaf_keys.end());
            return;
        }

        auto* internal_node = static_cast<InternalNode<T>*>(node);
        for (std::size_t i{}; i < internal_node->children_size(); i++) {
            collect_keys(internal_node->childAt(static_cast<int>(i)));
        }
    };

    collect_keys(root_.get());

    root_ = std::make_unique<LeafNode<T>>();
    for (const T& key : keys) {
        add(key);
    }
}

template <typename T>
std::vector<T> BPlusTree<T>::getRange(const T& lo, const T& hi) {
    std::vector<T> keys_in_range;
    if (lo > hi) {
        return keys_in_range;
    }

    // Descend to the first leaf that could contain the lower bound.
    Node<T>* curr_node = root_.get();
    while (curr_node != nullptr && !curr_node->isLeaf()) {
        auto* curr_internal = static_cast<InternalNode<T>*>(curr_node);
        int child_index = curr_internal->getIndex(lo);
        curr_node = curr_internal->childAt(child_index);
    }

    // Shift left while the previous leaf may still contain keys in range.
    LeafNode<T>* curr_leaf = static_cast<LeafNode<T>*>(curr_node);
    while (curr_leaf != nullptr && curr_leaf->prev() != nullptr) {
        std::vector<T> prev_values = curr_leaf->prev()->getList();
        if (prev_values.empty() || prev_values.back() < lo) {
            break;
        }
        curr_leaf = curr_leaf->prev();
    }

    // Once we are at the first relevant leaf, the sibling chain gives us an
    // ordered scan without revisiting internal nodes.
    while (curr_leaf != nullptr) {
        std::vector<T> values = curr_leaf->getList();
        for (const T& curr : values) {
            if (curr >= lo && curr <= hi) {
                keys_in_range.push_back(curr);
            }
            if (curr > hi) {
                return keys_in_range;
            }
        }
        curr_leaf = curr_leaf->next();
    }

    return keys_in_range;
}

template <typename T>
std::string BPlusTree<T>::toString() {
    std::string builder;
    std::queue<Node<T>*> q;
    q.push(root_.get());

    while (!q.empty()) {
        std::size_t q_size = q.size();
        for (std::size_t i{}; i < q_size; i++) {
            Node<T>* curr_node = q.front();
            q.pop();
            builder.append(curr_node->toString());

            if (!curr_node->isLeaf()) {
                auto* curr_internal = static_cast<InternalNode<T>*>(curr_node);
                for (std::size_t child_index{}; child_index < curr_internal->children_size(); child_index++) {
                    q.push(curr_internal->childAt(static_cast<int>(child_index)));
                }
            }
        }
        builder.append("\n");
    }

    return builder;
}
