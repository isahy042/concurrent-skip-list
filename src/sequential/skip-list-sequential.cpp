
#include "skip-list-sequential.h"
#include <cassert>

SequentialSkipList::SequentialSkipList(int total_elements){
    head = std::make_shared<Node>(nullptr, nullptr, INT_MIN); // header will never be removed
    max_levels_ = std::max(1, static_cast<int>(std::ceil(std::log2(total_elements)))); // logN levels

    // create tower of head nodes
    std::shared_ptr<Node> curr_node = head;
    for (int i = 0; i < max_levels_ - 1; i++){
        std::shared_ptr<Node> n = std::make_shared<Node>(nullptr, nullptr, INT_MIN);
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

bool SequentialSkipList::insert(int val){
    if (contains(val)) return false;

    std::shared_ptr<Node> update[max_levels_]; // used to mark where we would insert node at new level

    std::shared_ptr<Node> curr_node = head;
    
    int curr_level = max_levels_ - 1; // starting at the top level

    // fill out the update ptr array 
    while (curr_node) {
        if (curr_node->next && curr_node->next->value <= val) {
            // keep traversing right
            if (curr_node->next->value == val) {
                return false; // we don't need to insert.
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
    while ((coinflip() && curr_level < max_levels_) || curr_level == 0){  

        assert(update[curr_level] && "null ptr in update array when inserting"); // should not be nullptr

        new_node = std::make_shared<Node>(update[curr_level]->next, curr_node, val);
        update[curr_level]->next = new_node;
        curr_node = new_node;
        curr_level ++;
    }

    return true;
}
bool SequentialSkipList::remove(int val){
    if (!contains(val)) return false;
    std::shared_ptr<Node> update[max_levels_]; // used to mark where we would insert node at new level

    std::shared_ptr<Node> curr_node = head;
    int curr_level = max_levels_ - 1; // starting at the top level

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
    for (int i = 0; i < max_levels_; i++){
        // loop until we reach a level higher than the node's tower
        if (!update[i]) break;
        assert(update[i]->next->value == val && "Incorrect update array when removing");
        update[i]->next = update[i]->next->next;
    }

    return true;
}

/**
 * Validates the skip list to ensure it maintains all invariants (each layer is sorted and each column is equivalent).
 * Go from top to bottom to make sure for each layer, it is sorted and the element underneath is equivalent.   
 */
void SequentialSkipList::validate() {
    auto curr = head;
    
    for (int layer = max_levels_ - 1; layer >= 0; layer--) {
        std::shared_ptr<Node> temp_curr = curr;
        if (layer > 0) {
            curr = curr->down;
        }

        while(temp_curr) {
            // First check the node below is the same
            if (temp_curr->down) {
                ASSERT_MSG(temp_curr->down->value == temp_curr->value, "Node below is not the same");
            }

            // Check next node is greater than current one
            if (temp_curr->next) {
                ASSERT_MSG(temp_curr->next->value > temp_curr->value, "Next node is not greater than current one");
            }
            temp_curr = temp_curr->next;
        }
    }

    std::cout << "Validation successful" << std::endl;
}

