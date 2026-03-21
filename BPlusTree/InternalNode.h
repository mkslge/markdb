#ifndef INTERNALNODE_H
#define INTERNALNODE_H

#include <algorithm>
#include <cassert>
#include <vector>
#include "Node.h"

template <typename T>
class InternalNode : public Node<T> {
    private:
        std::vector<T> values_;
        std::vector<Node<T>*> children_;

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
            return children_[index];
        }

        void insertChild(Node<T>* child, int index) {
            assert(index >= 0 && index <= static_cast<int>(children_.size()));
            children_.insert(children_.begin() + index, child);
        }

        bool removeChild(Node<T>* child) {
            for(size_t i{}; i < children_.size();i++) {
                if(child == children_[i]) {
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

        const std::vector<Node<T>*>& children() const {
            return children_;
        }

        void setValues(const std::vector<T>& values) {
            values_ = values;
        }

        void setChildren(const std::vector<Node<T>*>& children) {
            children_ = children;
        }

        int indexOfChild(Node<T>* child) const {
            for (size_t i{}; i < children_.size(); i++) {
                if (children_[i] == child) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        void replaceChildWithSplit(int child_index, Node<T>* left_child, Node<T>* right_child) {
            assert(child_index >= 0 && child_index < static_cast<int>(children_.size()));
            children_.erase(children_.begin() + child_index);
            children_.insert(children_.begin() + child_index, right_child);
            children_.insert(children_.begin() + child_index, left_child);
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

        




};




#endif //INTERNALNODE_H
