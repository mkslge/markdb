//
// Created by Mark on 2/19/26.
//

#ifndef LRUREPLACER_H
#define LRUREPLACER_H

#include "Replacer.h"
#include "../../Page.h"
#include <list>




class LRUReplacer : public Replacer {
public:
    class Node {
    public:
        Node* next_;
        Node *prev_;
        int key_;

        Node(int key) : key_(key),  prev_(nullptr), next_(nullptr){};
    };




private:





    Node* dummy_start_;
    Node* dummy_end_;
    std::unordered_map<int, Node*> map_; //key = page_id, value = index in removable


public:
    LRUReplacer();
    bool victim(int *frame_id) override;

    void pin(int frame_id) override;
    void unpin(int frame_id) override;
    size_t size() override;

private:
    void deleteNode(Node* to_delete);

    void addToBack(Node *to_move);


};



#endif //LRUREPLACER_H
