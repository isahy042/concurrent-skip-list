
#include "skip-list-lock-free.h"
#include <cassert>

LockFreeSkipList::LockFreeSkipList(int total_elements){
    this->total_elements = total_elements;
    
    // head and tail will never be removed
    head = std::make_shared<LockFreeNode>(INT_MIN); 
    std::shared_ptr<LockFreeNode> tail = std::make_shared<LockFreeNode>(INT_MAX); 
    head->succ.right = tail;

    this->max_layers = std::max(1, static_cast<int>(std::log2(total_elements))); // logN levels

    // create tower of head and tail nodes
    std::shared_ptr<LockFreeNode> curr_head_node = head;
    std::shared_ptr<LockFreeNode> curr_tail_node = tail;

    for (int i = 0; i < max_levels - 1; i++){
        std::shared_ptr<LockFreeNode> h = std::make_shared<LockFreeNode>(INT_MIN);
        std::shared_ptr<LockFreeNode> t = std::make_shared<LockFreeNode>(INT_MAX);
        h->succ.right = t;

        curr_head_node->down = h;
        curr_tail_node->down = t;

        curr_head_node = h;
        curr_tail_node = t;

    }
}

bool LockFreeSkipList::contains(int val){
   return true;
}

void LockFreeSkipList::insert(int val){


}

void LockFreeSkipList::remove(int val){
    
}
