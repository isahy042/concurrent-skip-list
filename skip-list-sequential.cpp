
#include "skip-list-sequential.h"
#include <cassert>

SequentialSkipList::SequentialSkipList(int total_elements, int min_val){
    head = std::make_shared<Node>(nullptr, nullptr, min_val - 1); // header will never be removed
    max_levels = std::max(1, static_cast<int>(std::log2(total_elements))); // logN levels

    // create tower of head nodes
    std::shared_ptr<Node> curr_node = head;
    for (int i = 0; i < max_levels - 1; i++){
        std::shared_ptr<Node> n = std::make_shared<Node>(nullptr, nullptr, min_val - 1);
        curr_node->down = n;
        curr_node = n;
    }
}

bool SequentialSkipList::contains(int val){
    std::shared_ptr<Node> curr_node = head;

    while (curr_node) {
        if (curr_node->next && curr_node->next->value <= val) {
            // keep traversing right
            if (curr_node->next->value == val) {
                return true; // return top level node of the target val
            }
            curr_node = curr_node->next;
        } else{
            // drop down to next level
            curr_node = curr_node->down;
        }
    }

    return false; 
}

void SequentialSkipList::insert(int val){
    std::shared_ptr<Node> update[max_levels]; // used to mark where we would insert node at new level

    std::shared_ptr<Node> curr_node = head;
    
    int curr_level = max_levels - 1; // starting at the top level

    // fill out the update ptr array 
    while (curr_node) {
        if (curr_node->next && curr_node->next->value <= val) {
            // keep traversing right
            if (curr_node->next->value == val) {
                return; // we don't need to insert.
            }
            curr_node = curr_node->next;
        } else{
            update[curr_level] = curr_node;
            curr_level--;
            // drop down to next level
            curr_node = curr_node->down;

            assert(curr_level >= 0 || !curr_node);
        }
    }

    // create and insert nodes into the skip list
    curr_level = 0;
    std::shared_ptr<Node> new_node;
    curr_node = nullptr;

    // coinflip to determine how many levels to promote this new node for
    while ((coinflip() && curr_level < max_levels) || curr_level == 0){  

        assert(update[curr_level] && "null ptr in update array when inserting"); // should not be nullptr

        new_node = std::make_shared<Node>(update[curr_level]->next, curr_node, val);
        update[curr_level]->next = new_node;
        curr_node = new_node;
        curr_level ++;
    }

}
void SequentialSkipList::remove(int val){
    std::shared_ptr<Node> update[max_levels]; // used to mark where we would insert node at new level

    std::shared_ptr<Node> curr_node = head;
    int curr_level = max_levels - 1; // starting at the top level

    // fill out the update ptr array 
    while (curr_node) {
        if (curr_node->next && curr_node->next->value <= val) { // keep traversing right
            if (curr_node->next->value == val) { // if found on current level, drop down
                update[curr_level] = curr_node;
                curr_node = curr_node->down;
                curr_level--;
            } else {
                curr_node = curr_node->next;
            }
        } else{
            curr_level--;
            // drop down to next level
            curr_node = curr_node->down;
            assert(curr_level >= 0 || !curr_node);
        }
    }

    // remove nodes
    for (int i = 0; i < max_levels; i++){
        // loop until we reach a level higher than the node's tower
        if (!update[i]) break;
        assert(update[i]->next->value == val && "Incorrect update array when removing");
        update[i]->next = update[i]->next->next;
    }
}
