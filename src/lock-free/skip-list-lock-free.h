#pragma once

#include "../skip-list.h"
#include <atomic>

enum class RNode {
    VALID, 
    DUPLICATE_KEY, 
    NO_SUCH_NODE
};

enum class NodeStatus {
    IN, DELETED
};

const float EPISLON = 0.01f;

struct LockFreeNode {

    struct  Succ{
      LockFreeNode* right = nullptr;
      unsigned int mark = 0;
      unsigned int flag = 0;

      bool operator==(const Succ& other) {
        return right == other.right && mark == other.mark && flag == other.flag;
      }

    }; // see page 16 pf related publication
    std::atomic<Succ> succ{Succ()};

    RNode type = RNode::VALID; 

    LockFreeNode* back_link = nullptr; // if a node is not (about to be) marked deleted, this is null
    
    LockFreeNode* down = nullptr;
    LockFreeNode* up = nullptr; // only used by head node

    LockFreeNode* tower_root = nullptr;

    int key;

    LockFreeNode(int k): key(k) {};
    LockFreeNode(RNode r): type(r) {};

    ~LockFreeNode() = default;

    // retrieve info from atomic
    LockFreeNode* get_right(){  return succ.load().right;  }
    bool get_mark(){  return succ.load().mark;  }
    bool get_flag(){  return succ.load().flag;  }
    void set_succ(Succ s){  succ.store(s);  }
      
};

struct LockFreeNodePair {
  LockFreeNode* first;
  LockFreeNode* second;
};

struct FlagTuple{
  LockFreeNode* node;
   NodeStatus status;
  bool result;
};

class LockFreeSkipList : public SkipList
{
  public:
    LockFreeNode* head;

    LockFreeSkipList(int total_elements);
    ~LockFreeSkipList() = default;

    bool contains(int val);
    bool insert(int val);
    bool remove(int val);

    void validate();
    
    // search helper
    LockFreeNodePair search_to_level(float val, int level);
    std::pair<LockFreeNode*, int> find_start(int level);
    LockFreeNodePair search_right(float val, LockFreeNode* curr_node);

    // insertion & deletion helper
    LockFreeNodePair insert_node(LockFreeNode* new_node, LockFreeNode* prev_node, LockFreeNode* next_node);
    LockFreeNode* delete_node(LockFreeNode* prev_node, LockFreeNode* del_node);
    
    // flagging
    FlagTuple try_flag_node(LockFreeNode* prev_node, LockFreeNode* target_node);
    
    void help_marked(LockFreeNode* prev_node, LockFreeNode* del_node);
    void help_flagged(LockFreeNode* prev_node, LockFreeNode* del_node);
    void try_mark(LockFreeNode* del_node);

};