#pragma once

#include "skip-list.h"

enum class RNode {
    VALID, 
    DUPLICATE_KEY, 
    NO_SUCH_KEY, 
    NO_SUCH_NODE
};

enum class NodeStatus {
    IN, DELETED
};


struct LockFreeNode {

    struct Succ{
      std::shared_ptr<LockFreeNode> right;
      bool mark;
      bool flag;

      bool operator==(const Succ& other) {
        return right == other.right && mark == other.mark && flag == other.flag;
    }
    }; // see page 16 pf related publication
    Succ succ;

    RNode type = RNode::VALID; 

    std::shared_ptr<LockFreeNode> back_link; // if a node is not (about to be) marked deleted, this is null
    
    std::shared_ptr<LockFreeNode> down;
    std::shared_ptr<LockFreeNode> up; // only used by head node

    std::shared_ptr<LockFreeNode> tower_root;

    int key;

    LockFreeNode(int key): key(key) {};
    ~LockFreeNode() = default;
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
    int max_levels;

    LockFreeSkipList(int total_elements);
    ~LockFreeSkipList() = default;

    bool contains(int val);
    void insert(int val);
    void remove(int val);

    // search helper
    LockFreeNodePair search_to_level(int val, int level);
    std::pair<std::shared_ptr<LockFreeNode>, int> find_start(int level);
    LockFreeNodePair search_right(int val, std::shared_ptr<LockFreeNode> curr_node);

    // insertion & deletion helper
    LockFreeNodePair insert_node(std::shared_ptr<LockFreeNode> new_node, std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> next_node);
    std::shared_ptr<LockFreeNode> delete_node(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> del_node);
    
    // flagging
    FlagTuple try_flag_node(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> target_node);
    
    void help_marked(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> del_node);
    void help_flagged(std::shared_ptr<LockFreeNode> prev_node, std::shared_ptr<LockFreeNode> del_node);
    void try_mark(std::shared_ptr<LockFreeNode> del_node);


  private:
    int total_elements;
    int max_layers;

};
