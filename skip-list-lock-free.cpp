
#include "skip-list-lock-free.h"
#include <cassert>

//TODO: Make atomic
LockFreeNode::Succ CAS(LockFreeNode::Succ* a, const LockFreeNode::Succ old_val, const LockFreeNode::Succ new_val){
    if (*a == old_val){ 
        *a = new_val;
        return LockFreeNode::Succ{old_val.right,old_val.mark,old_val.flag};
    }
    return *a;
}


LockFreeSkipList::LockFreeSkipList(int total_elements){
    this->total_elements = total_elements;
    
    // head and tail will never be removed
    head = std::make_shared<LockFreeNode>(INT_MIN); 
    std::shared_ptr<LockFreeNode> tail = std::make_shared<LockFreeNode>(INT_MAX); 
    head->succ.right = tail;

    this->max_levels = std::max(1, static_cast<int>(std::log2(total_elements))); // logN levels

    // create tower of head and tail nodes
    std::shared_ptr<LockFreeNode> curr_head_node = head;
    std::shared_ptr<LockFreeNode> curr_tail_node = tail;

    for (int i = 0; i < max_levels - 1; i++){
        std::shared_ptr<LockFreeNode> h = std::make_shared<LockFreeNode>(INT_MIN);
        std::shared_ptr<LockFreeNode> t = std::make_shared<LockFreeNode>(INT_MAX);

        h->succ.right = t;

        curr_head_node->up = h;
        h->down = curr_head_node; // up ptr only used by head node
        //t->down = curr_tail_node;

        // tail towers contains only keys and nothing else
        // head and tail towers does not use tower root

        curr_head_node = h;
        //curr_tail_node = t;

    }

    curr_head_node->up = curr_head_node; // top level head node points to self

}

bool LockFreeSkipList::contains(int val){
    LockFreeNodePair pair = search_to_level(val, 1); 
    if (pair.second->key == val) return true;
    return false;
}

void LockFreeSkipList::insert(int val){

    LockFreeNodePair pair = search_to_level(val, 1);
    std::shared_ptr<LockFreeNode> prev_node = pair.first;
    std::shared_ptr<LockFreeNode> next_node = pair.second;
    
    if (prev_node->key == val) return false; // duplicate key

    std::shared_ptr<LockFreeNode> new_root = std::make_shared<LockFreeNode>(val);
    new_root->tower_root = new_root;

    std::shared_ptr<LockFreeNode> new_node = new_root; // node currently being inserted

    // determine target height
    int tower_height = 1;
    while (coinflip() && tower_height <= max_levels - 1){  
        tower_height++;
    }

    int curr_level = 1;

    while(true){ // insert node at curr_level
        pair = insert_node(new_node, prev_node, next_node);
        prev_node = pair.first;

        if (pair.second->type == RNode::DUPLICATE_KEY && curr_level == 1){
            // free new_node
            return true;
        }
        if (new_root->succ.mark == 1){
            if (pair.second == new_node && new_node != new_root){
                delete_node(prev_node, new_node);
            }
            return true;
        }

        curr_level ++;
        if (curr_level == tower_height + 1) return; // stop building tower
        
        std::shared_ptr<LockFreeNode> last_node = new_node;
        new_node = std::make_shared<LockFreeNode>(val);
        new_node->down = last_node;
        new_node->tower_root = new_root;

        pair = search_to_level(val, curr_level);
        prev_node = pair.first;
        next_node = pair.second;

    }

}

void LockFreeSkipList::remove(int val){
    LockFreeNodePair pair = search_to_level(val - EPISLON, 1); 
    if (pair.second->key != val)
        return false; // No such key
    if (delete_node(pair.first, pair.second)->type == RNode::NO_SUCH_NODE)
        return false; // No such key

    // delete nodes at higher level of tower
    search_to_level(val, 2);
    return true;
}


LockFreeNodePair LockFreeSkipList::search_to_level(float val, int level){

    std::shared_ptr<LockFreeNode> curr_node;
    int curr_v;
    std::pair<std::shared_ptr<LockFreeNode>, int> start = find_start(level); 
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
std::pair<std::shared_ptr<LockFreeNode>, int> LockFreeSkipList::find_start(int level){
    std::shared_ptr<LockFreeNode> curr_node = head;
    int curr_v = 1;

    while ((curr_node->up->succ.right->key != INT_MAX) || (curr_v < level)){
        curr_node = curr_node->up;
        curr_v ++;
    }

    return {curr_node, curr_v};
}


LockFreeNodePair LockFreeSkipList::search_right(float val, std::shared_ptr<LockFreeNode> curr_node){
    std::shared_ptr<LockFreeNode> next_node = curr_node->succ.right;
    while(next_node->key <= val){

        // tower superfluous, delete next_node
        while (next_node->tower_root->succ.mark == true){ 
            FlagTuple try_flag_tuple = try_flag_node(curr_node, next_node);
            curr_node = try_flag_tuple.node;
            if (try_flag_tuple.status == NodeStatus::IN) help_flagged(curr_node, next_node); // next_node's pred was flagged
            // keep going right until no longer superfluous
            next_node = curr_node->succ.right;
        }

        // proceed searching normally
        if (next_node->key <= val){
            curr_node = next_node;
            next_node = curr_node->succ.right;
        }

    }

    return {curr_node, next_node};
}

// insertion & deletion helper
LockFreeNodePair LockFreeSkipList::insert_node(std::shared_ptr<LockFreeNode> new_node, std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> next_node){
   
    if (prev_node->key == new_node->key) return {prev_node, std::make_shared<LockFreeNode>(RNode::DUPLICATE_KEY)};

    while(true){
        LockFreeNode::Succ prev_succ = prev_node->succ;
        if (prev_succ.flag == 1) {// if prev_node flagged
            help_flagged(prev_node, prev_succ.right); 
        }

        else {

            new_node->succ = LockFreeNode::Succ{next_node, 0, 0};
            LockFreeNode::Succ result = CAS(&prev_node->succ,{next_node,0,0}, {new_node,0,0});

            if (result == LockFreeNode::Succ{new_node,0,0}){ // CAS success
                return {prev_node, new_node};
            }
            else{
                if (result.mark == 0 && result.flag == 1){
                    help_flagged(prev_node, result.right);
                }
                while(prev_node->succ.mark == 1)
                    prev_node = prev_node->back_link;
            }
        }
        LockFreeNodePair pair = search_right(new_node->key, prev_node);
        prev_node = pair.first;
        next_node = pair.second;
        if (prev_node->key == new_node->key)
            return {prev_node, std::make_shared<LockFreeNode>(RNode::DUPLICATE_KEY)};
    }
}

std::shared_ptr<LockFreeNode> LockFreeSkipList::delete_node(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> del_node){
    FlagTuple flag_tuple = try_flag_node(prev_node, del_node);
    if (flag_tuple.status == NodeStatus::IN)
        help_flagged(prev_node, del_node);
    if (!flag_tuple.result)
        return std::make_shared<LockFreeNode>(RNode::NO_SUCH_NODE);
    return del_node;
}
    
// flagging
FlagTuple LockFreeSkipList::try_flag_node(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> target_node){
    LockFreeNode::Succ result;
    while (true){ // keep looping
        if (prev_node->succ == LockFreeNode::Succ{target_node, 0, 1}) // TODO: check comparison
            return {prev_node, NodeStatus::IN, false};

        result = CAS(&prev_node->succ, {target_node, 0, 0}, {target_node, 0, 1});

        if (result == LockFreeNode::Succ{target_node, 0, 0})  // sucess CAS
            return {prev_node, NodeStatus::IN, true};
        
        if (result == LockFreeNode::Succ{target_node, 0, 1}) // failed due to flagging
            return {prev_node, NodeStatus::IN, false}; 

        while (prev_node->succ.mark == 1) // possible failture due to marking
            prev_node = prev_node->back_link;

        LockFreeNodePair pair = search_right(target_node->key - EPISLON, prev_node); // TODO: we actually want key - epi see p69
        prev_node = pair.first;

        if (pair.second != target_node)
            return {prev_node, NodeStatus::DELETED, false}; 

    }

}
    
void LockFreeSkipList::help_marked(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> del_node){
    std::shared_ptr<LockFreeNode> next_node = del_node->succ.right;
    CAS(&prev_node->succ, {del_node,0,1}, {next_node,0,0});
}

void LockFreeSkipList::help_flagged(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> del_node){
    del_node->back_link = prev_node;
    if (del_node->succ.mark == 0)
        try_mark(del_node);
    help_marked(prev_node, del_node);
}

void LockFreeSkipList::try_mark(std::shared_ptr<LockFreeNode> del_node){
    std::shared_ptr<LockFreeNode> next_node;
    while(del_node->succ.mark != 1){
        next_node = del_node->succ.right;
        LockFreeNode::Succ result = CAS(&del_node->succ, {next_node,0,0}, {next_node,1,0});
        if (result.mark == 0 && result.flag == 1)
            help_flagged(del_node, result.right);
    }
}