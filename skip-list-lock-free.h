#pragma once

#include "skip-list.h"
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

    struct Succ{
      LockFreeNode* right;
      bool mark;
      bool flag;

      bool operator==(const Succ& other) {
        return right == other.right && mark == other.mark && flag == other.flag;
      }
      std::shared_ptr<LockFreeNode> get_right(){  return std::shared_ptr<LockFreeNode>(right);  }

    }; // see page 16 pf related publication
    std::atomic<Succ> succ;

    RNode type = RNode::VALID; 

    std::shared_ptr<LockFreeNode> back_link; // if a node is not (about to be) marked deleted, this is null
    
    std::shared_ptr<LockFreeNode> down;
    std::shared_ptr<LockFreeNode> up; // only used by head node

    std::shared_ptr<LockFreeNode> tower_root;

    int key;

    LockFreeNode(int k): key(k) {};
    LockFreeNode(RNode r): type(r) {};

    ~LockFreeNode() = default;

    // retrieve info from atomic
    std::shared_ptr<LockFreeNode> get_right(){  return std::shared_ptr<LockFreeNode>(succ.load().right);  }
    bool get_mark(){  return succ.load().mark;  }
    bool get_flag(){  return succ.load().flag;  }
    void set_succ(Succ s){  succ.store(s);  }
    void set_succ(std::shared_ptr<LockFreeNode> ptr, bool m, bool f){  
      Succ s = Succ{ptr.get(), m, f};
      succ.store(s);  
    }


    
    
      
};

struct LockFreeNodePair {
  std::shared_ptr<LockFreeNode> first;
  std::shared_ptr<LockFreeNode> second;
};

struct FlagTuple{
  std::shared_ptr<LockFreeNode> node;
   NodeStatus status;
  bool result;
};

class LockFreeSkipList : public SkipList
{
  public:
    std::shared_ptr<LockFreeNode> head;

    LockFreeSkipList(int total_elements);
    ~LockFreeSkipList() = default;

    bool contains(int val);
    bool insert(int val);
    bool remove(int val);

    // search helper
    LockFreeNodePair search_to_level(float val, int level);
    std::pair<std::shared_ptr<LockFreeNode>, int> find_start(int level);
    LockFreeNodePair search_right(float val, std::shared_ptr<LockFreeNode> curr_node);

    // insertion & deletion helper
    LockFreeNodePair insert_node(std::shared_ptr<LockFreeNode> new_node, std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> next_node);
    std::shared_ptr<LockFreeNode> delete_node(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> del_node);
    
    // flagging
    FlagTuple try_flag_node(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> target_node);
    
    void help_marked(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> del_node);
    void help_flagged(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> del_node);
    void try_mark(std::shared_ptr<LockFreeNode> del_node);

};
