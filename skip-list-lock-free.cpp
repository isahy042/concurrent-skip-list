
#include "skip-list-lock-free.h"
#include <cassert>

LockFreeNode::Succ CAS(std::atomic<LockFreeNode::Succ>* a, const LockFreeNode::Succ old_val, const LockFreeNode::Succ new_val) {

    LockFreeNode::Succ expected = a->load();
    // Compare and swap using std::atomic's compare_exchange_strong
    if (a->compare_exchange_strong(expected, new_val)) {
        return expected; // Return old value before the update
    }
    return a->load(); // If CAS fails, return the current value of succ
}


LockFreeSkipList::LockFreeSkipList(int total_elements){

    // head and tail will never be removed
    head = new LockFreeNode(INT_MIN);
    LockFreeNode* tail = new LockFreeNode(INT_MAX); 
    
    //head->succ.right = tail;
    head->set_succ({tail, head->get_mark(), head->get_flag()});

    max_levels_ = std::max(1, static_cast<int>(std::ceil(std::log2(total_elements)))); // logN levels

    // create tower of head and tail nodes
    LockFreeNode* curr_head_node = head;

    for (int i = 0; i < max_levels_ - 1; i++){
        LockFreeNode* h = new LockFreeNode(INT_MIN);
        LockFreeNode* t = new LockFreeNode(INT_MAX);

        //h->succ.right = t;
        h->set_succ({t, h->get_mark(), h->get_flag()});

        curr_head_node->up = h;
        h->down = curr_head_node; // up ptr only used by head node

        // tail towers contains only keys and nothing else
        // head and tail towers does not use tower root

        curr_head_node = h;

    }

    curr_head_node->up = curr_head_node; // top level head node points to self

}

bool LockFreeSkipList::contains(int val){
    LockFreeNodePair pair = search_to_level(val, 1); 
    if (pair.second->key == val) return true;
    return false;
}

bool LockFreeSkipList::insert(int val){

    LockFreeNodePair pair = search_to_level(val, 1);
    LockFreeNode* prev_node = pair.first;
    LockFreeNode* next_node = pair.second;
    
    if (prev_node->key == val) return false; // duplicate key

    LockFreeNode* new_root = new LockFreeNode(val);
    new_root->tower_root = new_root;

    LockFreeNode* new_node = new_root; // node currently being inserted

    // determine target height
    int tower_height = 1;
    while (coinflip() && tower_height <= max_levels_ - 1){  
        tower_height++;
    }

    int curr_level = 1;

    while(true){ // insert node at curr_level
        pair = insert_node(new_node, prev_node, next_node);
        prev_node = pair.first;

        if (pair.second->type == RNode::DUPLICATE_KEY && curr_level == 1){
            // free new_node
            std::cout << "duplicate key, new node freed \n" << std::flush;
            //delete new_node;
            return false;
        }
        if (new_root->get_mark() == 1){
            if (pair.second == new_node && new_node != new_root){
                delete_node(prev_node, new_node);
            }
            return true;
        }

        curr_level ++;
        if (curr_level == tower_height + 1) return true; // stop building tower
        
        LockFreeNode* last_node = new_node;
        new_node = new LockFreeNode(val);
        new_node->down = last_node;
        new_node->tower_root = new_root;

        pair = search_to_level(val, curr_level);
        prev_node = pair.first;
        next_node = pair.second;

    }


}

bool LockFreeSkipList::remove(int val){
    LockFreeNodePair pair = search_to_level(val - EPISLON, 1); // TODO: shold be val - epi

    if (pair.second->key != val)
        return false; // No such key
    if (delete_node(pair.first, pair.second)->type == RNode::NO_SUCH_NODE)
        return false; // No such key

    // delete nodes at higher level of tower
    search_to_level(val, 2);
    return true;
}


LockFreeNodePair LockFreeSkipList::search_to_level(float val, int level){

    LockFreeNode* curr_node;
    int curr_v;
    std::pair<LockFreeNode*, int> start = find_start(level); 
    curr_node = start.first;
    curr_v = start.second;

    while (start.second > val){
        LockFreeNodePair pair = search_right(val, curr_node);
        curr_node = pair.first->down;
        curr_v --;
    }

    return search_right(val, curr_node);
}

/* Finds the upper most starting head node below a given level */
std::pair<LockFreeNode*, int> LockFreeSkipList::find_start(int level){
    LockFreeNode* curr_node = head;
    int curr_v = 1;

    while ((curr_node->up->get_right()->key != INT_MAX) || (curr_v < level)){
        curr_node = curr_node->up;
        curr_v ++;
    }

    return {curr_node, curr_v};
}


LockFreeNodePair LockFreeSkipList::search_right(float val, LockFreeNode* curr_node){
    LockFreeNode* next_node = curr_node->get_right();
    while(next_node->key <= val){

        // tower superfluous, delete next_node
        while (next_node->tower_root->get_mark() == true){ 
            FlagTuple try_flag_tuple = try_flag_node(curr_node, next_node);
            curr_node = try_flag_tuple.node;
            if (try_flag_tuple.status == NodeStatus::IN) help_flagged(curr_node, next_node); // next_node's pred was flagged
            // keep going right until no longer superfluous
            next_node = curr_node->get_right();
        }

        // proceed searching normally
        if (next_node->key <= val){
            curr_node = next_node;
            next_node = curr_node->get_right();
        }

    }

    return {curr_node, next_node};
}

// insertion & deletion helper
LockFreeNodePair LockFreeSkipList::insert_node(LockFreeNode* new_node, LockFreeNode* prev_node, LockFreeNode* next_node){
   
    if (prev_node->key == new_node->key) return {prev_node, new LockFreeNode(RNode::DUPLICATE_KEY)};

    while(true){
        LockFreeNode::Succ prev_succ = prev_node->succ;
        if (prev_succ.flag == 1) {// if prev_node flagged
            help_flagged(prev_node, prev_succ.right); 
        }

        else {

            //new_node->succ = LockFreeNode::Succ{next_node, 0, 0};
            new_node->set_succ({next_node,0,0});
            LockFreeNode::Succ result = CAS(&prev_node->succ,{next_node,0,0}, {new_node,0,0});

            if (result == LockFreeNode::Succ{new_node,0,0}){ // CAS success
                return {prev_node, new_node};
            }
            else{
                if (result.mark == 0 && result.flag == 1){
                    help_flagged(prev_node, result.right);
                }
                while(prev_node->get_mark() == 1)
                    prev_node = prev_node->back_link;
            }
        }
        LockFreeNodePair pair = search_right(new_node->key, prev_node);
        prev_node = pair.first;
        next_node = pair.second;
        if (prev_node->key == new_node->key)
            return {prev_node, new LockFreeNode(RNode::DUPLICATE_KEY)};
    }
}

LockFreeNode* LockFreeSkipList::delete_node(LockFreeNode* prev_node, LockFreeNode* del_node){
    FlagTuple flag_tuple = try_flag_node(prev_node, del_node);
    if (flag_tuple.status == NodeStatus::IN)
        help_flagged(prev_node, del_node);
    if (!flag_tuple.result)
        return new LockFreeNode(RNode::NO_SUCH_NODE);
    return del_node;
}
    
// flagging
FlagTuple LockFreeSkipList::try_flag_node(LockFreeNode* prev_node, LockFreeNode* target_node){
    LockFreeNode::Succ result;
    while (true){ // keep looping
        if (prev_node->succ.load() == LockFreeNode::Succ{target_node, 0, 1}) // TODO: check comparison
            return {prev_node, NodeStatus::IN, false};

        result = CAS(&prev_node->succ, {target_node, 0, 0}, {target_node, 0, 1});

        if (result == LockFreeNode::Succ{target_node, 0, 0})  // sucess CAS
            return {prev_node, NodeStatus::IN, true};
        
        if (result == LockFreeNode::Succ{target_node, 0, 1}) // failed due to flagging
            return {prev_node, NodeStatus::IN, false}; 

        while (prev_node->get_mark() == 1) // possible failture due to marking
            prev_node = prev_node->back_link;

        LockFreeNodePair pair = search_right(target_node->key - EPISLON, prev_node); // TODO: we actually want key - epi see p69
        prev_node = pair.first;

        if (pair.second != target_node)
            return {prev_node, NodeStatus::DELETED, false}; 

    }

}
    
void LockFreeSkipList::help_marked(LockFreeNode* prev_node, LockFreeNode* del_node){
    LockFreeNode* next_node = del_node->get_right();
    CAS(&prev_node->succ, {del_node,0,1}, {next_node,0,0});
}

void LockFreeSkipList::help_flagged(LockFreeNode* prev_node, LockFreeNode* del_node){
    del_node->back_link = prev_node;
    if (del_node->get_mark() == 0)
        try_mark(del_node);
    help_marked(prev_node, del_node);
}

void LockFreeSkipList::try_mark(LockFreeNode* del_node){
    LockFreeNode* next_node;
    while(del_node->get_mark() != 1){
        next_node = del_node->get_right();
        LockFreeNode::Succ result = CAS(&del_node->succ, {next_node,0,0}, {next_node,1,0});
        if (result.mark == 0 && result.flag == 1)
            help_flagged(del_node, result.right);
    }
}