#pragma once

#include <cmath>
#include <iostream>
#include <vector>
#include <memory>


inline bool coinflip(){
    return (rand() & 10) >= 5; // 50 50 chance
}

/* a single skip list operation */ 
struct Operation {
  char op; // insert, remove, contains
  int value;
};

/* a single skip list node */ 
struct Node {
  std::shared_ptr<Node> next;
  std::shared_ptr<Node> down;
  int value;
  Node(std::shared_ptr<Node> n, std::shared_ptr<Node> d, int v) : next(n), down(d), value(v) {}
};

/* Skip List Interface */
class SkipList
{
  public:
    SkipList(){}

    // Node* head; // start of skip list
    // int max_levels;

    /* whether the skip list contains a node with the target value*/
    virtual bool contains(int val) = 0;
    /* Add a node with the target value into the skip list, duplicates not allowed */
    virtual void insert(int val) = 0;
    /* Remove the node with the target value from the skip list */
    virtual void remove(int val) = 0;
};

class Checker{
  public:

  Checker(char mode, int num_ops, int min_val, bool v);

  SkipList* skiplist;
  bool verbose;

  void RunOperations(std::vector<Operation>* ops);
  void PrintOutcome();
  std::string SkipListToString();
};

const char *get_option_string(const char *option_name,
                              const char *default_value);
int get_option_int(const char *option_name, int default_value);
float get_option_float(const char *option_name, float default_value);
