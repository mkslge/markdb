#ifndef INTERNALNODE_H
#define INTERNALNODE_H

#include "LeafNode.h"
#include "Node.h"
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

template <typename T>
class InternalNode : public Node<T> {
private:
    std::vector<T> values_;
    std::vector<std::unique_ptr<Node<T>>> children_;

public:
    InternalNode() = default;

    /*
     * Preconditions:
     * - `values_` is sorted in ascending order.
     *
     * Postconditions:
     * - `val` is inserted into `values_` in sorted order.
     * - Returns the index where the separator was inserted.
     */
    int add(const T& val) override {
        for (std::size_t i{}; i < values_.size(); i++) {
            if (val < values_[i]) {
                values_.insert(values_.begin() + static_cast<long>(i), val);
                return static_cast<int>(i);
            }
        }

        values_.push_back(val);
        return static_cast<int>(values_.size() - 1);
    }

    bool remove(const T& val) {
        for (std::size_t i{}; i < values_.size(); i++) {
            if (values_[i] == val) {
                values_.erase(values_.begin() + static_cast<long>(i));
                return true;
            }
        }

        return false;
    }

    T at(int index) override {
        assert(index >= 0 && index < static_cast<int>(values_.size()));
        return values_[index];
    }

    Node<T>* childAt(int index) {
        assert(index >= 0 && index < static_cast<int>(children_.size()));
        return children_[index].get();
    }

    /*
     * Preconditions:
     * - `index` is within `[0, children_.size()]`.
     * - `child` owns a valid subtree.
     *
     * Postconditions:
     * - Ownership of `child` is transferred into `children_` at `index`.
     */
    void insertChild(std::unique_ptr<Node<T>> child, int index) {
        assert(index >= 0 && index <= static_cast<int>(children_.size()));
        children_.insert(children_.begin() + index, std::move(child));
    }

    bool removeChild(Node<T>* child) {
        for (std::size_t i{}; i < children_.size(); i++) {
            if (child == children_[i].get()) {
                children_.erase(children_.begin() + static_cast<long>(i));
                return true;
            }
        }

        return false;
    }

    bool isLeaf() override {
        return false;
    }

    bool contains(const T& val) override {
        return std::find(values_.begin(), values_.end(), val) != values_.end();
    }

    std::size_t size() override {
        return values_.size();
    }

    std::size_t children_size() const {
        return children_.size();
    }

    std::vector<Node<T>*> children() const {
        std::vector<Node<T>*> raw_children;
        raw_children.reserve(children_.size());
        for (const auto& child : children_) {
            raw_children.push_back(child.get());
        }
        return raw_children;
    }

    void setValues(const std::vector<T>& values) {
        values_ = values;
    }

    void setChildren(std::vector<std::unique_ptr<Node<T>>> children) {
        children_ = std::move(children);
    }

    std::vector<std::unique_ptr<Node<T>>> takeChildren() {
        return std::move(children_);
    }

    int indexOfChild(Node<T>* child) const {
        for (std::size_t i{}; i < children_.size(); i++) {
            if (children_[i].get() == child) {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    /*
     * Preconditions:
     * - `child_index` identifies an existing child entry.
     * - `left_child` and `right_child` are the two children created by a split.
     *
     * Postconditions:
     * - The old child at `child_index` is removed.
     * - `left_child` and `right_child` are inserted in its place.
     */
    void replaceChildWithSplit(int child_index,
                               std::unique_ptr<Node<T>> left_child,
                               std::unique_ptr<Node<T>> right_child) {
        assert(child_index >= 0 && child_index < static_cast<int>(children_.size()));
        children_.erase(children_.begin() + child_index);
        children_.insert(children_.begin() + child_index, std::move(right_child));
        children_.insert(children_.begin() + child_index, std::move(left_child));
    }

    std::string toString() override {
        std::string builder;
        for (const T& value : values_) {
            builder.append(std::to_string(value) + " ");
        }
        return builder;
    }

    std::vector<T> getList() override {
        return values_;
    }

    int getIndex(const T& val) override {
        for (std::size_t i{}; i < values_.size(); i++) {
            if (val < values_[i]) {
                return static_cast<int>(i);
            }
        }

        return values_.empty() ? 0 : static_cast<int>(values_.size() - 1);
    }
};

#endif //INTERNALNODE_H
