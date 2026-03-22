#ifndef INTERNALNODE_H
#define INTERNALNODE_H

#include <algorithm>
#include <cassert>
#include <vector>
#include <memory>
#include "Node.h"
#include "LeafNode.h"

template <typename T>
class InternalNode : public Node<T> {
    private:
        std::vector<T> values_;
        std::vector<std::unique_ptr<Node<T>>> children_;

    public:
        InternalNode() {};
        int add(const T& val) {
            for(size_t i{}; i < values_.size();i++) {
                if(val < values_[i]) {
                    values_.insert(values_.begin() + i, val);
                    return i;
                }
            }
            values_.push_back(val);
            return values_.size() - 1;
        }



        T at(const int index) {
            assert(index >= 0 && index < static_cast<int>(values_.size()));
            return values_[index];
        }

        Node<T>* childAt(const int index) {
            assert(index >= 0 && index < static_cast<int>(children_.size()));
            return children_[index].get();
        }

        void insertChild(std::unique_ptr<Node<T>> child, int index) {
            assert(index >= 0 && index <= static_cast<int>(children_.size()));
            children_.insert(children_.begin() + index, std::move(child));
        }

        bool removeChild(Node<T>* child) {
            for(size_t i{}; i < children_.size();i++) {
                if(child == children_[i].get()) {
                    children_.erase(children_.begin() + i);
                    return true;
                }
            }
            return false;
        }

        

        bool isLeaf() {
            return false;
        }

        bool contains(const T& val) {
            return std::find(values_.begin(), values_.end(), val) != values_.end();
        }

        size_t size() { return values_.size(); };

        size_t children_size() {return children_.size(); };

        std::vector<Node<T>*> children() const {
            std::vector<Node<T>*> raw_children;
            raw_children.reserve(children_.size());
            for(size_t i{}; i < children_.size();i++) {
                raw_children.push_back(children_[i].get());
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
            for (size_t i{}; i < children_.size(); i++) {
                if (children_[i].get() == child) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        void replaceChildWithSplit(int child_index,
                                   std::unique_ptr<Node<T>> left_child,
                                   std::unique_ptr<Node<T>> right_child) {
            assert(child_index >= 0 && child_index < static_cast<int>(children_.size()));
            children_.erase(children_.begin() + child_index);
            children_.insert(children_.begin() + child_index, std::move(right_child));
            children_.insert(children_.begin() + child_index, std::move(left_child));
        }

        std::string toString() {
            std::string builder = "";
            for(T& value : values_) {
                builder.append(std::to_string(value) + " ");
            }
            return builder;
        }

        std::vector<T> getList() {
            return this->values_;
        }

        int getIndex(const T& val) {
            for(size_t i{}; i < values_.size();i++) {
                if(val < values_[i]) {
                    return i;
                }
            }
            return values_.size() - 1;
        }

        




};




#endif //INTERNALNODE_H
